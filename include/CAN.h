#ifndef CAN_H
#define CAN_H

#include "driver/twai.h"

// Renaming enums and structs for an easier experience
typedef enum {
    CAN_MODE_NORMAL = TWAI_MODE_NORMAL,
    CAN_MODE_NO_ACK = TWAI_MODE_NO_ACK,
    CAN_MODE_LISTEN_ONLY = TWAI_MODE_LISTEN_ONLY
} CAN_MODE_t;

typedef twai_general_config_t can_general_config_t;
typedef twai_timing_config_t can_timing_config_t;
typedef twai_filter_config_t can_filter_config_t;

typedef enum {
    CAN_TIMING_CONFIG_1KBITS() = TWAI_TIMING_CONFIG_1KBITS();

};

#endif