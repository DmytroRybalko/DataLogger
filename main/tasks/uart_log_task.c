#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"

#include "app.h"
#include "tasks.h"
#include "app_types.h"
#include "app_config.h"

void uart_log_task(void *arg) {
    
    sink_msg_t m;
    log_record_t *r = &(m.rec);
    char line[80];

    for (;;) {
        if (xQueueReceive(uart_log_q, &m, portMAX_DELAY) != pdTRUE) continue;
        
        snprintf(line, sizeof line,
            "time=%04u-%02u-%02u %02u:%02u:%02u temp=%d.%02dC hum=%u.%02u%% press=%u mmHg\r\n",
            r->year, r->month, r->day,
            r->hour, r->min, r->sec,
            r->temp_x100 / 100, abs(r->temp_x100 % 100),
            r->hum_x100 / 100, r->hum_x100 % 100,
            r->press_mmhg
        );

        uart_write_bytes(UART_PORT, line, strlen(line));
    }
}