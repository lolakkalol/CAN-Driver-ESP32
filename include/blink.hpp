/**
 * @file blink.h
 * @author Alexander Stenlund (alexander.stenlund@telia.com)
 * @brief Defines the pins used for LEDs and Buttons on the steering 
 * wheel aswell as some function to link an LED to a button. This is 
 * intended to be used togheter with an ISR for the buttons. This is 
 * written using the Arduino framework and is made with the intention 
 * to run on a ESP32-WROOM-32E.
 * @version 0.1
 * @date 2022-06-25
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef BLINK_H
#define BLINK_H

#include <Arduino.h>

/**
 * @brief The buttons used have quite severe button bouncing and 
 * hence a "dead" time is added that ignores button presses from the 
 * same button if it the first press was less than 150ms (default 
 * value can be changed) ago.
 * 
 */
#define DEBOUNCE_TIME 150

/// LED GPIO pins defined on the board
enum LED_PIN {
    PIN_Left_LED = 25,    /// The GPIO pin for left indicator LED
    PIN_Hazard_LED = 26,  /// The GPIO pin for hazard indicator LED
    PIN_Right_LED = 27,   /// The GPIO pin for right indicator LED
    PIN_Battery_LED = 33  /// The GPIO pin for battery warning indicator LED
};


/// Button GPIO pins defined on the board
enum BUTTON_PIN {
    PIN_Left_Button = 17,  /// The GPIO pin for left indicator button
    PIN_Hazard_Button = 16, /// The GPIO pin for hazard indicator button
    PIN_Right_Button = 4   /// The GPIO pin for right indicator button
};

/// ISR ID's used for struct identification in the ISR
enum ISR_ID {
    ISR_Linker,
};

/**
 * @brief ISR_ID is used in the ISR to identify the type of argument
 * that was passed. This requires each argument that is passed to 
 * have a ISR_ID in the beginning of its struct.
 * 
 */
struct isr_arg {
  ISR_ID ID;
};

// Tells what button should toggle what LED
struct Linker {
  ISR_ID     ID;
  uint32_t debounce_time;
  BUTTON_PIN btn_pin;
  LED_PIN    LED_pin;

  Linker(BUTTON_PIN x, LED_PIN y) : 
    ID(ISR_Linker), 
    debounce_time(millis()),
    btn_pin(x), 
    LED_pin(y) {}
};

/**
 * @brief Toggles the LED GPIO pin HIGH or LOW depending on its state. 
 * HIGH is set LOW and LOW is set HIGH.
 * 
 * @param pin: The LED GPIO pin number.
 */
void toggleLED(LED_PIN pin);


/**
 * @brief Toggles only one LED at a time. To switch the LED being 
 * toggled change nextLED. When switching LED this function also turns
 * off the old LED.
 * 
 * @param nextLED The next LED to start blinking.
 * @param blinkingLED The LED blinking, used to keep track of the LED blinking.
 */
void blinker(Linker** nextLED, Linker** blinkingLED);

/* ----------------------------------------------------------- */

#define BLINKER_SPEED 500 // Time in ms

void init_blink();
void checkIfBlink();

extern SemaphoreHandle_t blinkSemaphore;

// Keeps track of what LED to blink and what LED was being blinked
extern Linker* blink;
extern Linker* newBlink;

/* ----------------------------------------------------------- */

#endif