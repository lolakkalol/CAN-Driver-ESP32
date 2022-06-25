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
#include "blink.h"

// Defining LEDs to be used in the program
LED Left_LED = {PIN_Left_LED};
LED Hazard_LED = {PIN_Hazard_LED};
LED Right_LED = {PIN_Right_LED};
LED Battery_LED = {PIN_Battery_LED};

// Defining Buttons to be used in the program
Button Left_Button = {PIN_Left_Button, false};
Button Hazard_Button = {PIN_Hazard_Button, false};
Button Right_Button = {PIN_Right_Button, false};

/*
 * Creating an array of tuples that pairs buttons and LEDs. This 
 * structures which button is controlling what LED
 */
btnLED_Tuple arr_LED_btn[] = {
  btnLED_Tuple {&Left_Button, &Left_LED},
  btnLED_Tuple {&Hazard_Button, &Hazard_LED},
  btnLED_Tuple {&Right_Button, &Right_LED}
};

// Description at the definition of the functions
void ARDUINO_ISR_ATTR isr(void* arg);
void toggleLED(LED_PIN pin);


/// Main setup of the program runs only once at the begining
void setup() {

  // LEDs GPIO pin config
  pinMode(PIN_Left_LED, OUTPUT);
  pinMode(PIN_Hazard_LED, OUTPUT);
  pinMode(PIN_Right_LED, OUTPUT);

  // Buttons GPIO pin config, the board has internal pull down resistors
  pinMode(PIN_Left_Button, INPUT_PULLDOWN);
  pinMode(PIN_Hazard_Button, INPUT_PULLDOWN);
  pinMode(PIN_Right_Button, INPUT_PULLDOWN);

  // Attach interrupts to button GPIOs, interrupt on falling edge
  attachInterruptArg(PIN_Left_Button, isr, &Left_Button, FALLING);
  attachInterruptArg(PIN_Hazard_Button, isr, &Hazard_Button, FALLING);
  attachInterruptArg(PIN_Right_Button, isr, &Right_Button, FALLING);

 /* 
  * Starts serial communication, can be used to print text to pc 
  * terminal and should be removed before release
  */
  Serial.begin(115200);
}

// Main program loop, will run forever
void loop() {

  // Calls the blink controller and passes the LED and button tupple
  blinkController(arr_LED_btn, 3);

  delay(100);

}