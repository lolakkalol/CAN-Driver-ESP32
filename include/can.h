#ifndef CAN_H
#define CAN_H

#include "driver/twai.h"

/**
 * @brief Initilizes and starts the CAN controller for 500kbps in
 * normal mode on PIN 22 as TX and 23 as RX.
 * 
 * @return esp_err_t Error message from driver 
 */
esp_err_t config_and_start_twai();

#endif