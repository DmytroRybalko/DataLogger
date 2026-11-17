#include <string.h>
#include "driver/uart.h"
#include "esp_log.h"

#include "app.h"
#include "app_config.h"


static void print_help(void)
{
    printf("Commands:\n"
           "  help\n"
           "  status\n"
           "  read\n"
           "  log on\n"
           "  log off\n"
           "  erase\n");
}

static void parse_line(char *line)
{
    char *p = line;
    while (*p == ' ' || *p == '\t') p++;

    if (*p == '\0')
        return;

    if (strcmp(p, "help") == 0) {
        print_help();
        return;
    }

    if (strcmp(p, "status") == 0) {
        printf("status: log=%s flash_full=%s\n",
               (xEventGroupGetBits(flags) & F_LOG_ON) ? "on" : "off",
               (xEventGroupGetBits(flags) & F_FLASH_FULL) ? "yes" : "no");
        return;
    }

    if (strcmp(p, "read") == 0) {
        sample_req_t req = { .src = REQ_UART, .log = false };
        xQueueOverwrite(request_q, &req);
        printf("sample requested\n");
        return;
    }

    if (strcmp(p, "log on") == 0) {
        xEventGroupSetBits(flags, F_LOG_ON);
        printf("logging enabled\n");
        return;
    }

    if (strcmp(p, "log off") == 0) {
        xEventGroupClearBits(flags, F_LOG_ON);
        printf("logging disabled\n");
        return;
    }

    if (strcmp(p, "erase") == 0) {
        sink_msg_t m = { .type = MSG_ERASE };
        xQueueSend(flash_q, &m, 0);
        printf("erase requested\n");
        return;
    }

    printf("unknown command: %s\n", p);
    print_help();
}

void uart_cmd_task(void *arg)
{
    char line[UART_CMD_LINE_MAX];
    size_t n = 0;

    for (;;) {
        uint8_t ch;
        int got = uart_read_bytes(UART_PORT, &ch, 1, pdMS_TO_TICKS(100));
        if (got <= 0) {
            continue;
        }

        if (ch == '\r' || ch == '\n') {
            if (n > 0) {
                line[n] = '\0';
                parse_line(line);
                n = 0;
            }
        } else if (n < sizeof(line) - 1) {
            line[n++] = (char)ch;
        }
    }
}