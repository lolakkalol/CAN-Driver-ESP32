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
  PIN_Left_Button = 16,  /// The GPIO pin for left indicator button
  PIN_Hazard_Button = 4, /// The GPIO pin for hazard indicator button
  PIN_Right_Button = 0   /// The GPIO pin for right indicator button
};

/// ISR ID's used for struct identification in the ISR
enum ISR_ID {
  ISR_BUTTON,
  ISR_LED,
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

/// A struct for LEDs
struct LED {
  ISR_ID ID;    /// Used to identify the struct in ISR
  LED_PIN pin;  /// The GPIO pin on the physical board

  /// Initilizer list, always wants LED struct to have the same ISR_ID
  LED(LED_PIN x) : ID(ISR_LED), pin(x) {}
};

/// A struct for buttons
struct Button {
  ISR_ID ID;              /// Used to identify the struct in the ISR
  uint32_t debounce_time; /// Keeps track of last time it was pressed
  BUTTON_PIN pin;         /// GPIO pin on the physical board
  bool pressed;           /// Keeps track of toggle state not button state

  Button(BUTTON_PIN x, bool y) : 
    ID(ISR_BUTTON), 
    debounce_time(millis()), 
    pin(x), pressed(y) {}
};

/**
 * @brief Tuple of a button and an LED. The button is going to when 
 * passed into blinkController toggle if the LED is flashing or not.
 * 
 */
struct btnLED_Tuple {
  Button* p_btn;
  LED* p_LED;
};

/**
 * @brief Toggles the LED GPIO pin HIGH or LOW depending on its state. 
 * HIGH is set LOW and LOW is set HIGH.
 * 
 * @param pin: The LED GPIO pin number.
 */
void toggleLED(LED_PIN pin);

/**
 * @brief Looks at the buttons provided to see if they have been 
 * toggled and if so starts blinking the LED in the same tuple as 
 * the button.
 * 
 * @param tuple: An array of tuples, each tuple contain the types Button and LED.
 * @param size : The length of the tuple array.
 */
void blinkController(btnLED_Tuple tuple[], int size);

/**
 * @brief The interrupt service rutine for the program, gets called 
 * whenever an interrupt occurs. It handles button presses/interrupts 
 * and a debouncing delay is implemented.
 * 
 * @param arg: A pointer to an argument.
 */
void ARDUINO_ISR_ATTR isr(void* arg);

#endif