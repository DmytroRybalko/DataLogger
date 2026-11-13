#include "driver/uart.h"

void uart_cmd_task(void *arg) {
    /* TODO: RX -> help/status/read/flush/erase/log on|off/period N/dump */
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}