// main/app_config.h
// Central place for tunable constants. Values that you may want to change
// without editing code can be overridden from Kconfig (CONFIG_APP_*).
#pragma once

#include "app_types.h"   // log_record_t (needed for the static asserts below)

/* ------------------------------------------------------------------ */
/* Timing                                                              */
/* ------------------------------------------------------------------ */

// Measurement cadence (software timer period)
#ifdef CONFIG_APP_SAMPLE_PERIOD_MS
#define SAMPLE_PERIOD_MS          CONFIG_APP_SAMPLE_PERIOD_MS
#else
#define SAMPLE_PERIOD_MS          5000
#endif

// Store only every N-th measurement to flash (1 = every measurement).
// Display and UART log still get every measurement.
#ifdef CONFIG_APP_LOG_EVERY_N_SAMPLES
#define LOG_EVERY_N_SAMPLES       CONFIG_APP_LOG_EVERY_N_SAMPLES
#else
#define LOG_EVERY_N_SAMPLES       1
#endif

// How long sampler waits on request_q before refreshing its heartbeat.
// Must be much shorter than the WDT timeout.
#define SAMPLER_WAIT_MS           1000

/* ------------------------------------------------------------------ */
/* Watchdog                                                            */
/* ------------------------------------------------------------------ */

// wdt_task waits this long for heartbeats of all critical tasks.
// The hardware/Task WDT timeout itself is set in sdkconfig
// (CONFIG_ESP_TASK_WDT_TIMEOUT_S, for example 15) and must be larger.
#define WDT_SUPERVISOR_PERIOD_MS  10000

/* ------------------------------------------------------------------ */
/* I2C bus                                                             */
/* ------------------------------------------------------------------ */

#define I2C_PORT                  0
#define I2C_SDA_GPIO              8       // example, change for your board
#define I2C_SCL_GPIO              9       // example, change for your board
#define I2C_FREQ_HZ               100000

#define I2C_XFER_TIMEOUT_MS       100     // single transaction
#define I2C_MUTEX_TIMEOUT_MS      100     // waiting for the bus mutex
#define I2C_RETRY_COUNT           3       // retries before bus recovery

#define BME280_I2C_ADDR           0x76    // 0x77 if SDO is tied high
#define DS3231_I2C_ADDR           0x68
#define SSD1306_I2C_ADDR          0x3C

/* ------------------------------------------------------------------ */
/* Display                                                             */
/* ------------------------------------------------------------------ */

#define DISPLAY_WIDTH             128
#define DISPLAY_HEIGHT            64

/* ------------------------------------------------------------------ */
/* UART                                                                */
/* ------------------------------------------------------------------ */

#define UART_PORT                 0
#define UART_BAUD                 115200
#define UART_RX_BUF_SIZE          256
#define UART_TX_BUF_SIZE          512
#define UART_CMD_LINE_MAX         32      // longest accepted command line

/* ------------------------------------------------------------------ */
/* Flash log                                                           */
/* ------------------------------------------------------------------ */

#define LOG_PARTITION_LABEL       "logdata"
#define FLASH_SECTOR_SIZE         4096

#define RECORDS_PER_SECTOR        (FLASH_SECTOR_SIZE / sizeof(log_record_t))

_Static_assert(sizeof(log_record_t) == 16,
               "log_record_t must stay 16 bytes (record layout on flash)");
_Static_assert(FLASH_SECTOR_SIZE % sizeof(log_record_t) == 0,
               "a sector must hold a whole number of records");

/* ------------------------------------------------------------------ */
/* Queues                                                              */
/* ------------------------------------------------------------------ */

#define REQUEST_Q_LEN             1       // overwrite, requests merge
#define DISPLAY_Q_LEN             1       // overwrite, only latest value
#ifdef CONFIG_APP_FLASH_Q_LEN
#define FLASH_Q_LEN               CONFIG_APP_FLASH_Q_LEN
#else
#define FLASH_Q_LEN               16      // absorbs sector erase/write time
#endif
#define UART_LOG_Q_LEN            8

/* ------------------------------------------------------------------ */
/* Tasks: priority and stack size (bytes)                              */
/* ------------------------------------------------------------------ */

#define SAMPLER_TASK_PRIO         5
#define FLASH_TASK_PRIO           4
#define DISPLAY_TASK_PRIO         3
#define UART_LOG_TASK_PRIO        2
#define UART_CMD_TASK_PRIO        2
#define WDT_TASK_PRIO             2

#define SAMPLER_TASK_STACK        4096
#define FLASH_TASK_STACK          4096
#define DISPLAY_TASK_STACK        4096
#define UART_LOG_TASK_STACK       3072
#define UART_CMD_TASK_STACK       3072
#define WDT_TASK_STACK            2048

// Check real usage with uxTaskGetStackHighWaterMark() and trim afterwards.