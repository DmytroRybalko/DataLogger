#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"

#include "app_config.h"
#include "app_types.h"


extern QueueHandle_t     request_q;     // Requests from timer/UART to read BME280 data
extern QueueHandle_t     display_q;    // Messages to display task (OLED output)
extern QueueHandle_t     flash_q;      // Messages for flash/log storage task
extern QueueHandle_t     uart_log_q;   // Log messages sent to UART logger
extern QueueHandle_t     uart_cmd_q;   // UART command requests from console

extern SemaphoreHandle_t i2c_mutex;    // Protects shared I2C bus access
extern SemaphoreHandle_t uart_mutex;   // Protects UART log/console access

extern EventGroupHandle_t hb_group;    // Heartbeat flags for watchdog/task health monitoring
extern EventGroupHandle_t flags;       // Runtime feature flags such as logging enabled/disabled

extern TimerHandle_t sample_timer;     // Triggers periodic sensor sampling every 5 s
