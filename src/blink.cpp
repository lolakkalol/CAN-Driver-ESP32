#include <queue>
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

void blinker(Linker** nextLED, Linker** blinkingLED) {

  if (*nextLED != NULL) {
    if (*nextLED == *blinkingLED && *blinkingLED != NULL) {
      digitalWrite((*blinkingLED)->LED_pin, LOW);
      *nextLED = NULL;
      *blinkingLED = NULL;
    }

    if (*blinkingLED != NULL)
      digitalWrite((*blinkingLED)->LED_pin, LOW);
    *blinkingLED = *nextLED;
    *nextLED = NULL;
  }

  if (*blinkingLED != NULL)
    toggleLED((*blinkingLED)->LED_pin);
}