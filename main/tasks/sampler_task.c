#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"

#include "app.h"

// Fake data for testing display format
static void fake_sample(log_record_t *r) {
    r->year = 2026;
    r->month = 11;
    r->day = 13;
    r->hour = 12;
    r->min = 34;
    r->sec = 56;

    r->temp_x100 = 2345;   // 23.45 C
    r->hum_x100 = 4567;    // 45.67 %
    r->press_mmhg = 1013;
    r->status = 0;
}


/**
 * @brief Main sampling loop that waits for a sample request, reads sensor data,
 * then distributes the result to the display, UART logger, and flash queue.
 */
void sampler_task(void *arg) {
    sample_req_t req;
    for (;;) {
        xEventGroupSetBits(hb_group, HB_SAMPLER);                          // (1) heartbeat for WDT
        if (xQueueReceive(request_q, &req, pdMS_TO_TICKS(1000)) != pdTRUE) // (2) task sleeps here
            continue;                                                      //     timeout: only refresh heartbeat

        log_record_t r = {0};
        fake_sample(&r);
        //ds3231_read(&r);                                                   // (3) read RTC timestamp
        //bme280_read(&r);                                                   //     read BME280 sensor data with retry/recovery

        sink_msg_t m = { .type = MSG_RECORD, .rec = r };                     // (4) send record to sinks
        xQueueOverwrite(display_q, &m);
        //xQueueSend(uart_log_q, &m, 0);
        if (req.log && (xEventGroupGetBits(flags) & F_LOG_ON))
            xQueueSend(flash_q, &m, 0);
    }                                                                       // (5) loop back to (1)
}