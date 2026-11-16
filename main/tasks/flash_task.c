// Consumes sink_msg_t from flash_q:
//   MSG_RECORD - append to the one-sector RAM buffer, write the sector when it is full
//   MSG_FLUSH  - write the partially filled buffer (tail padded with 0xFF)
//   MSG_ERASE  - erase the whole partition and start from the first sector
// Build only ONE flash_task.c variant (this one or the extended one).

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_partition.h"

#include "app_config.h"
#include "app_types.h"      // log_record_t, sink_msg_t, F_*, HB_*
#include "app.h"

static const char *TAG = "flash";

static const esp_partition_t *s_part;
static log_record_t s_buf[RECORDS_PER_SECTOR];   // one flash sector in RAM
static size_t s_count;                           // records currently in s_buf
static size_t s_off;                             // next sector to write (kept in RAM only)

static void write_sector(void)
{
    // Pad the unused tail with 0xFF so empty space can be told from data.
    memset(&s_buf[s_count], 0xFF, (RECORDS_PER_SECTOR - s_count) * sizeof(log_record_t));
    s_count = 0;                                  // the buffer is consumed in every case

    esp_err_t e = esp_partition_erase_range(s_part, s_off, FLASH_SECTOR_SIZE);
    if (e == ESP_OK) {
        e = esp_partition_write(s_part, s_off, s_buf, FLASH_SECTOR_SIZE);
    }
    if (e != ESP_OK) {
        ESP_LOGE(TAG, "sector @%u: %s", (unsigned)s_off, esp_err_to_name(e));
        xEventGroupSetBits(flags, F_FLASH_FULL);  // any error: stop logging
        return;
    }

    ESP_LOGI(TAG, "sector @%u written", (unsigned)s_off);
    s_off += FLASH_SECTOR_SIZE;
    if (s_off + FLASH_SECTOR_SIZE > s_part->size) {
        xEventGroupSetBits(flags, F_FLASH_FULL);  // raised after the write: nothing is lost
        ESP_LOGI(TAG, "Partition is full: %u", (unsigned)(s_off));
    }
}

static void erase_all(void)
{
    for (size_t off = 0; off < s_part->size; off += FLASH_SECTOR_SIZE) {
        if (esp_partition_erase_range(s_part, off, FLASH_SECTOR_SIZE) != ESP_OK) {
            ESP_LOGE(TAG, "erase @%u failed", (unsigned)off);
            return;
        }
        xEventGroupSetBits(hb_group, HB_FLASH);   // long operation: keep the heartbeat alive
    }
    s_off = 0;
    s_count = 0;
    xEventGroupClearBits(flags, F_FLASH_FULL);
    ESP_LOGI(TAG, "partition erased");
}

void flash_task(void *arg)
{
    s_part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
                                      ESP_PARTITION_SUBTYPE_ANY,
                                      LOG_PARTITION_LABEL);
    ESP_ERROR_CHECK(s_part ? ESP_OK : ESP_ERR_NOT_FOUND);   // fail fast on a wrong partition table
    
    // TEST ONLY!!!
    esp_err_t err = esp_partition_erase_range(s_part, 0, s_part->size);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Partition erased!");
    } else {
        ESP_LOGE(TAG, "Erase failed: %s", esp_err_to_name(err));
    }
    
    sink_msg_t m;
    for (;;) {
        xEventGroupSetBits(hb_group, HB_FLASH);                          // heartbeat
        if (xQueueReceive(flash_q, &m, pdMS_TO_TICKS(1000)) != pdTRUE) continue;

        switch (m.type) {
        case MSG_RECORD:
            if (xEventGroupGetBits(flags) & F_FLASH_FULL) break;         // logging is stopped
            s_buf[s_count++] = m.rec;
            if (s_count == RECORDS_PER_SECTOR) write_sector();
            break;

        case MSG_FLUSH:
            if (s_count > 0) write_sector();                             // empty buffer: no sector consumed
            break;

        case MSG_ERASE:
            erase_all();
            break;

        default:
            break;
        }
    }
}