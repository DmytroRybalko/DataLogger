idf_component_register(
    SRCS
        "main.c"
        "tasks/sampler_task.c"
        "tasks/display_task.c"
        "tasks/flash_task.c"
        "tasks/uart_log_task.c"
        "tasks/uart_cmd_task.c"
        "tasks/wdt_task.c"
    INCLUDE_DIRS "."
    REQUIRES esp_driver_i2c esp_driver_uart bme280 esp_ssd1306
)