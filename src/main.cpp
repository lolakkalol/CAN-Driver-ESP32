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


/**
 * @brief The buttons used have quite severe button bouncing and 
 * hence a "dead" time is added that ignores button presses from the 
 * same button if it the first press was less than 150ms (default 
 * value can be changed) ago.
 * 
 */
#define DEBOUNCE_TIME 150


/// ISR ID's used for struct identification in the ISR
enum ISR_ID {
  ISR_BUTTON,
  ISR_LED,
};


/// LED GPIO pins
enum LED_PIN {
  PIN_Left_LED = 25,    /// The GPIO pin for left indicator LED
  PIN_Hazard_LED = 26,  /// The GPIO pin for hazard indicator LED
  PIN_Right_LED = 27,   /// The GPIO pin for right indicator LED
  PIN_Battery_LED = 33  /// The GPIO pin for battery warning indicator LED
};


/// Button GPIO pins
enum BUTTON_PIN {
  PIN_Left_Button = 16,  /// The GPIO pin for left indicator button
  PIN_Hazard_Button = 4, /// The GPIO pin for hazard indicator button
  PIN_Right_Button = 0   /// The GPIO pin for right indicator button
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
struct btn_LED_Tuple {
  Button* p_btn;
  LED* p_LED;
};


// Defining LEDs to be used in the program
LED Left_LED = {PIN_Left_LED};
LED Hazard_LED = {PIN_Hazard_LED};
LED Right_LED = {PIN_Right_LED};
LED Battery_LED = {PIN_Battery_LED};

// Defining Buttons tp be used in the program
Button Left_Button = {PIN_Left_Button, false};
Button Hazard_Button = {PIN_Hazard_Button, false};
Button Right_Button = {PIN_Right_Button, false};

/*
 * Creating an array of tupples that pairs buttons and LEDs. This 
 * structures which button is controlling what LED
 */
btn_LED_Tuple arr_LED_btn[] = {
  btn_LED_Tuple {&Left_Button, &Left_LED},
  btn_LED_Tuple {&Hazard_Button, &Hazard_LED},
  btn_LED_Tuple {&Right_Button, &Right_LED}
};

// Description at the definition of the functions
void ARDUINO_ISR_ATTR isr(void* arg);
void toggleLED(LED_PIN pin);
void blinkController(btn_LED_Tuple tuple[], int size);


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


/**
 * @brief Looks at the buttons provided to see if they have been 
 * toggled and if so starts blinking the LED in the same tuple as 
 * the button.
 * 
 * @param tuple: An array of tuples, each tuple contain the types Button and LED.
 * @param size : The length of the tuple array.
 */
void blinkController(btn_LED_Tuple tuple[], int size) {
  for(int i = 0; i < 3; i++) {
    if(tuple[i].p_btn->pressed == true)
      toggleLED(tuple[i].p_LED->pin);

    else
      digitalWrite(tuple[i].p_LED->pin, LOW);
        
  }
}


/**
 * @brief The interrupt service rutine for the program, gets called 
 * whenever an interrupt occurs.
 * 
 * @param arg: A pointer to an argument.
 */
void ARDUINO_ISR_ATTR isr(void* arg) {
  isr_arg* ptr = static_cast<isr_arg*> (arg);

  // Checks if the argument is a struct of type BUTTON
  if (ptr->ID == ISR_BUTTON) {
    Button* btn = static_cast<Button*> (arg);

    // Prevents button bouncing by ignoring rappid button presses.
    if (millis() - btn->debounce_time > DEBOUNCE_TIME) {
        btn->debounce_time = millis();
        btn->pressed = !(btn->pressed);
    }

  }

  return;
}


/**
 * @brief Toggles the LED GPIO pin HIGH or LOW depending on its state. 
 * HIGH is set LOW and LOW is set HIGH.
 * 
 * @param pin: The LED GPIO pin number.
 */
void toggleLED(LED_PIN pin) {
  if (digitalRead(pin) == HIGH)
    digitalWrite(pin, LOW);

  else
    digitalWrite(pin, HIGH);

  return;
}