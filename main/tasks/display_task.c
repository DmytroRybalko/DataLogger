#include <stdlib.h>
#include "esp_log.h"
#include "app.h"

static const char *TAG = "display";

static void draw_mock(const sink_msg_t *m)
{
    const log_record_t *r = &m->rec;

    /* TODO: ssd1306_clear(); ssd1306_print(...); ssd1306_update(); */
    // ESP_LOGI(TAG,
    //          "time=%04u-%02u-%02u %02u:%02u:%02u temp=%d.%02dC hum=%u.%02u%% press=%u mmHg",
    //          r->year, r->month, r->day,
    //          r->hour, r->min, r->sec,
    //          r->temp_x100 / 100, abs(r->temp_x100 % 100),
    //          r->hum_x100 / 100, r->hum_x100 % 100,
    //          r->press_mmhg);
}


void display_task(void *arg) {
    sink_msg_t m;
    for (;;) {
        if (xQueueReceive(display_q, &m, portMAX_DELAY) != pdTRUE) continue;
        // TODO: take i2c_mutex -> малювати T/H/P, час, статус LOG/FULL -> give
        draw_mock(&m);
    }
}