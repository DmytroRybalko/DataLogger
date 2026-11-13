#include "driver/uart.h"
#include "esp_log.h"

#include "app.h"

void uart_log_task(void *arg) {
    /* TODO: uart_log_q -> форматування рядка -> uart_mutex -> TX */
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}