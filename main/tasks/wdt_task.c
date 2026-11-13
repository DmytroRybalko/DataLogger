#include "esp_task_wdt.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#include "app.h"

void wdt_task(void *arg) {
    esp_task_wdt_add(NULL);                          // TWDT, таймаут у конфігурації ~15 с
    for (;;) {
        EventBits_t b = xEventGroupWaitBits(hb_group, HB_CRITICAL, pdTRUE, pdTRUE, pdMS_TO_TICKS(10000));
        if ((b & HB_CRITICAL) == HB_CRITICAL) esp_task_wdt_reset();   // інакше не годуємо -> reset
    }
}