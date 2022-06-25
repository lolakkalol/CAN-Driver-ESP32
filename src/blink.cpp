#include "blink.h"

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


/**
 * @brief Looks at the buttons provided to see if they have been 
 * toggled and if so starts blinking the LED in the same tuple as 
 * the button.
 * 
 * @param tuple: An array of tuples, each tuple contain the types Button and LED.
 * @param size : The length of the tuple array.
 */
void blinkController(btnLED_Tuple tuple[], int size) {
  for(int i = 0; i < 3; i++) {
    if(tuple[i].p_btn->pressed == true)
      toggleLED(tuple[i].p_LED->pin);

    else
      digitalWrite(tuple[i].p_LED->pin, LOW);
        
  }
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