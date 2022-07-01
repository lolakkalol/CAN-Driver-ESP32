/**
 * @file main.cpp
 * @author Alexander Stenlund (alexander.stenlund@telia.com)
 * @brief This file handles button interraction, ISR code, LED
 *  blinking/logic. This is written using the Arduino framework and
 *  is made with the intention to run on a ESP32-WROOM-32E.
 *  At the moment this code is far from complete. The documentation 
 * used for the arduino framework is the following: 
 * https://docs.espressif.com/projects/arduino-esp32/en/latest/index.html
 * 
 * @version 0.1
 * @date 2022-06-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "blink.hpp"
#include "can.h"

void setup() {
  Serial.begin(115200);
  config_and_start_twai();
  init_blink();

}


void loop() {

  receive_can_message();
  checkIfBlink();

}