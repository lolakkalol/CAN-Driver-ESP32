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

#include <Arduino.h>
#include <queue>
#include "blink.hpp"
#include "can.h"
#include "driver/twai.h" // Might need #include "driver/gpio.h" aswell

#define BLINKER_SPEED 500 // Time in ms
#define OWN_CAN_ID 0x37A3 // The MCU:s CAN ID

// Handle to the hardware timer
hw_timer_t* blinkTimer = NULL;

// Flags when the blinkerTimer alarm has gone off
SemaphoreHandle_t blinkSemaphore;

// Keeps track of what LED to blink and what LED was being blinked
Linker* blink = NULL;
Linker* newBlink = NULL;

uint32_t lastBlink = millis();

// Defining linkers
Linker Left_indicator(PIN_Left_Button, PIN_Left_LED);
Linker Hazard_indicator(PIN_Hazard_Button, PIN_Hazard_LED);
Linker Right_indicator(PIN_Right_Button, PIN_Right_LED);

// Function declarations
void ARDUINO_ISR_ATTR isr(void* arg);
void ARDUINO_ISR_ATTR timer_isr();

/// Main setup of the program runs only once at the begining
void setup() {

   /* 
  * Starts serial communication, can be used to print text to pc 
  * terminal and should be removed before release
  */
  Serial.begin(115200);

  // Starts can in Normal mode on pin 22 (tx) and 23 (rx), 500kbps 
  CAN_start();

  // LEDs GPIO pin config
  pinMode(PIN_Left_LED, OUTPUT);
  pinMode(PIN_Hazard_LED, OUTPUT);
  pinMode(PIN_Right_LED, OUTPUT);

  // Buttons GPIO pin config, the board has internal pull down resistors
  pinMode(PIN_Left_Button, INPUT_PULLDOWN);
  pinMode(PIN_Hazard_Button, INPUT_PULLDOWN);
  pinMode(PIN_Right_Button, INPUT_PULLDOWN);

  // Attach interrupts to button GPIOs, interrupt on falling edge
  attachInterruptArg(PIN_Left_Button, isr, &Left_indicator, FALLING);
  attachInterruptArg(PIN_Hazard_Button, isr, &Hazard_indicator, FALLING);
  attachInterruptArg(PIN_Right_Button, isr, &Right_indicator, FALLING);

  // Timer setup
  blinkTimer = timerBegin(0, 80, true);
  timerAttachInterrupt(blinkTimer, &timer_isr, true);
  timerAlarmWrite(blinkTimer, BLINKER_SPEED * 1000, true);
  timerAlarmEnable(blinkTimer);

  // Create a binary semaphore, (Can only flag high or low)
  blinkSemaphore = xSemaphoreCreateBinary();
}

// Main program loop, will run forever
void loop() {

  // Tries to take the semaphone and returns one if there is one to take
  if(xSemaphoreTake(blinkSemaphore, 0) == 1)
    blinker(&newBlink, &blink);

}

/* --------------------- TASKS ----------------------- */
static void can_receive_task(void* arg) {
  uint32_t* alerts;
  twai_read_alerts(alerts, portMAX_DELAY);

  // TODO: READ messages received and start task
}

/* -------------------- FUNCTIONS -------------------- */

void ARDUINO_ISR_ATTR timer_isr() {
  // Gives a semaphore signaling that the timer has gone off
  xSemaphoreGiveFromISR(blinkSemaphore, NULL);
}

/**
 * @brief The interrupt service rutine for the program, gets called 
 * whenever an interrupt occurs. It handles button presses/interrupts 
 * and a debouncing delay is implemented.
 * 
 * @param arg: A pointer to an argument.
 */
void ARDUINO_ISR_ATTR isr(void* arg) {
  isr_arg* ptr = static_cast<isr_arg*> (arg);

  // Checks if the argument is a struct of type BUTTON
  if (ptr->ID == ISR_Linker) {
    Linker* link = static_cast<Linker*> (arg);

    // Prevents button bouncing by ignoring rappid button presses.
    if (millis() - link->debounce_time > DEBOUNCE_TIME) {
        link->debounce_time = millis();
        newBlink = link;
    }

  }

  return;
}