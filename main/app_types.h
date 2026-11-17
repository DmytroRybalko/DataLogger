#ifndef APP_TYPES_H
#define APP_TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifndef BIT0
#define BIT0 (1U << 0)
#endif

#ifndef BIT1
#define BIT1 (1U << 1)
#endif

#define F_LOG_ON      BIT0
#define F_FLASH_FULL  BIT1
#define HB_SAMPLER    BIT0
#define HB_FLASH      BIT1
#define HB_CRITICAL   (HB_SAMPLER | HB_FLASH)

/**
 * @brief Packed measurement record stored for one sensor sample.
 */
typedef struct __attribute__((packed)) {
    uint16_t year;
    uint8_t  month, day, hour, min, sec;
    int16_t  temp_x100;
    uint16_t hum_x100;
    uint16_t press_mmhg;
    uint8_t  status;
    uint8_t  reserved[2];
} log_record_t;

/**
 * @brief Source that triggered a sampling request.
 */
typedef enum {
    REQ_TIMER,
    REQ_UART
} req_src_t;

/**
 * @brief Request sent to the sampling task.
 */
typedef struct {
    req_src_t src;
    bool log;
} sample_req_t;

/**
 * @brief Action type for messages sent to the sink tasks.
 */
typedef enum {
    MSG_RECORD,
    MSG_ERASE
} msg_type_t;

/**
 * @brief Message sent to a log/flash sink task.
 */
typedef struct {
    msg_type_t type;
    log_record_t rec;
} sink_msg_t;

#endif /* APP_TYPES_H */
