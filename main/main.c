#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2c_master.h"

//#include "bme280.h"
//#include "bme_280.h"

//#include <ssd1306.h>
//#include "font_latin_8x8.h"
//#include "mqtt_app.h"

#include "app.h"
#include "tasks/tasks.h"

#define I2C_MASTER_SCL_IO           GPIO_NUM_4       /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           GPIO_NUM_5       /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              I2C_NUM_0        /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ          (100000)         /*!< I2C master clock frequency */
#define I2C_MASTER_TIMEOUT_MS       1000

#define BME280_ADDR                 0x76            /*!< Address of the bme280 sensor */

QueueHandle_t      request_q;
QueueHandle_t      display_q;
QueueHandle_t      flash_q;
QueueHandle_t      uart_log_q;
QueueHandle_t      uart_cmd_q;
SemaphoreHandle_t  i2c_mutex;
SemaphoreHandle_t  uart_mutex;
EventGroupHandle_t hb_group;
EventGroupHandle_t flags;
TimerHandle_t      sample_timer;


static void sample_timer_cb(TimerHandle_t t) {
    sample_req_t r = { REQ_TIMER, true };
    xQueueOverwrite(request_q, &r);                 
}


// i2c master bus config
i2c_master_bus_config_t bus_config = {
    .i2c_port = I2C_MASTER_NUM,
    .sda_io_num = I2C_MASTER_SDA_IO,
    .scl_io_num = I2C_MASTER_SCL_IO,
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .glitch_ignore_cnt = 7,
    .flags.enable_internal_pullup = true,
    };

// BME280 sample config
// i2c_device_config_t bme280_config = {
//     .dev_addr_length = I2C_ADDR_BIT_LEN_7,
//     .device_address = BME280_ADDR,
//     .scl_speed_hz = I2C_MASTER_FREQ_HZ,
// };

void app_main(void)
{
    // --- INITIALIZATION SECTION --- //
    periph_init();

    i2c_mutex  = xSemaphoreCreateMutex();
    uart_mutex = xSemaphoreCreateMutex();
    request_q  = xQueueCreate(1,  sizeof(sample_req_t));
    display_q  = xQueueCreate(1,  sizeof(sink_msg_t));
    flash_q    = xQueueCreate(16, sizeof(sink_msg_t));
    uart_log_q = xQueueCreate(8,  sizeof(sink_msg_t));
    uart_cmd_q = xQueueCreate(8,  sizeof(sink_msg_t));
    hb_group   = xEventGroupCreate();                   // Heartbeat flags for watchdog monitoring
    flags      = xEventGroupCreate();                   // Runtime flags, e.g. whether logging is enabled
    
    xEventGroupSetBits(flags, F_LOG_ON);                // Enable log output by default

    xTaskCreate(sampler_task,  "sample",   4096, NULL, 5, NULL);
    xTaskCreate(flash_task,    "flash",    FLASH_TASK_STACK, NULL, 4, NULL);
    xTaskCreate(display_task,  "display",  4096, NULL, 3, NULL);
    xTaskCreate(uart_log_task, "uart_log", 3072, NULL, 2, NULL);
    xTaskCreate(uart_cmd_task, "uart_cmd", 3072, NULL, 2, NULL);
    xTaskCreate(wdt_task,      "wdt",      2048, NULL, 2, NULL);

    sample_timer = xTimerCreate("sample", pdMS_TO_TICKS(500), pdTRUE, NULL, sample_timer_cb);
    xTimerStart(sample_timer, 0);

    // Write first measurement before first timer period elaplsed
    sample_req_t first = { REQ_TIMER, true };
    xQueueOverwrite(request_q, &first);
}

