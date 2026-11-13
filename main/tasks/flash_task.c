#include "app.h"

void flash_task(void *arg) {
    sink_msg_t m;
    for (;;) {
        xEventGroupSetBits(hb_group, HB_FLASH);
        if (xQueueReceive(flash_q, &m, pdMS_TO_TICKS(1000)) != pdTRUE) continue;
        switch (m.type) {
        case MSG_RECORD: /* TODO: у RAM-буфер; повний -> erase+write; кінець розділу -> F_FLASH_FULL */ break;
        case MSG_FLUSH:  /* TODO: дописати неповний сектор */ break;
        case MSG_ERASE:  /* TODO: стерти розділ, скинути буфер і F_FLASH_FULL */ break;
        }
    }
}