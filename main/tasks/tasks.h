/**
 * @brief Reads sensor data on request and dispatches samples to display, log, and flash sinks.
 */
void sampler_task(void *arg);

/**
 * @brief Stores log records to the flash backend and erases the partition when needed.
 */
void flash_task(void *arg);

/**
 * @brief Updates the OLED display with the latest reading or status information.
 */
void display_task(void *arg);

/**
 * @brief Sends log messages over UART.
 */
void uart_log_task(void *arg);

/**
 * @brief Handles incoming UART commands and configuration requests.
 */
void uart_cmd_task(void *arg);

/**
 * @brief Monitors task heartbeat signals and resets the watchdog if the system is healthy.
 */
void wdt_task(void *arg);