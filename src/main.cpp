
/*
 * main.c
 *
 *  Created on: Jun 24, 2022
 *      Author: Alexander Stenlund
 *  
 *  Documentation used: https://docs.espressif.com/projects/arduino-esp32/en/latest/index.html
 */

#include <Arduino.h>

#define DEBOUNCE_TIME 150 // 200ms debounce time, ignores presses of the same button 200ms after the first press, can be adjusted.

enum ISR_ID {
  ISR_BUTTON,
};

enum LED_PIN {
  PIN_Left_LED = 25,  // The GPIO pin for left indicator LED
  PIN_Hazard_LED = 26,// The GPIO pin for hazard indicator LED
  PIN_Right_LED = 27,  // The GPIO pin for right indicator LED
  PIN_Battery_LED = 33  // The GPIO pin for battery warning indicator LED
};

enum BUTTON_PIN {
  PIN_Left_Button = 16,  // The GPIO pin for left indicator button
  PIN_Hazard_Button = 4,// The GPIO pin for hazard indicator button
  PIN_Right_Button = 0  // The GPIO pin for right indicator button
};

struct isr_arg {
  ISR_ID ID;
};

struct LED {
  LED_PIN pin;
  bool toggle; // If true the LED should be blinking
};

struct Button {
  ISR_ID ID;      // Only used in ISR to identify the type of struct as to not type cast the wrong type
  uint32_t debounce_time;
  BUTTON_PIN pin; // This is the GPIO pin the button sits at, can be used as an ID
  bool pressed;   // Toggles true/false when pressed

  Button(BUTTON_PIN x, bool y) : ID(ISR_BUTTON), debounce_time(millis()), pin(x), pressed(y) {}
};

// Defining Buttons
Button Left_Button = {PIN_Left_Button, false};
Button Hazard_Button = {PIN_Hazard_Button, false};
Button Right_Button = {PIN_Right_Button, false};

void ARDUINO_ISR_ATTR isr(void* arg); // This is the interrupt service rutine for buttons if an argument is supplied

void setup() {
  // LEDs GPIO config
  pinMode(PIN_Left_LED, OUTPUT);
  pinMode(PIN_Hazard_LED, OUTPUT);
  pinMode(PIN_Right_LED, OUTPUT);

  // Buttons GPIO config
  pinMode(PIN_Left_Button, INPUT_PULLDOWN);
  pinMode(PIN_Hazard_Button, INPUT_PULLDOWN);
  pinMode(PIN_Right_Button, INPUT_PULLDOWN);

  // Attach interrupts to button GPIOs
  attachInterruptArg(PIN_Left_Button, isr, &Left_Button, FALLING);
  attachInterruptArg(PIN_Hazard_Button, isr, &Hazard_Button, FALLING);
  attachInterruptArg(PIN_Right_Button, isr, &Right_Button, FALLING);

  // Start serial COM, Only used for serial monitor
  Serial.begin(115200);
}

void loop() {

  delay(100);

}

// Interrupt service rutine for interrupts supplied with arguments
void ARDUINO_ISR_ATTR isr(void* arg) {
  isr_arg* ptr = static_cast<isr_arg*> (arg);

  //If alot of different calls are made into the ISR remake this into a switch case
  if (ptr->ID == ISR_BUTTON) {
    Button* btn = static_cast<Button*> (arg);

    if (millis() - btn->debounce_time > DEBOUNCE_TIME) {
        btn->debounce_time = millis();
        Serial.println(btn->pressed);
        btn->pressed = !(btn->pressed);
    }

  }

  return;
}