#include <string.h>
#include "driver/uart.h"
#include "driver/gpio.h"

#include "app.h"
#include "app_config.h"

void uart_init(void)
{
    uart_config_t cfg = {
            .baud_rate  = UART_BAUD,
            .data_bits  = UART_DATA_8_BITS,
            .parity     = UART_PARITY_DISABLE,
            .stop_bits  = UART_STOP_BITS_1,
            .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
            .source_clk = UART_SCLK_DEFAULT
    };

    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, UART_RX_BUF_SIZE, UART_TX_BUF_SIZE, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &cfg));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
}


esp_err_t periph_init(void) {
    // ESP_RETURN_ON_ERROR(i2c_bus_init(), ...);   // першим: створює шину і мʼютекс
    // ds3231_init();
    // bme280_init();
    // ssd1306_init();
    uart_init();
    // log_flash_init();           // сканування розділу
    return ESP_OK;
}