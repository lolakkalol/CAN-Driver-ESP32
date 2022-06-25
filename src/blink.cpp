#include <queue>
#include "blink.hpp"

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
 * @brief Toggles only one LED at a time. To switch the LED being 
 * toggled change nextLED. When switching LED this function also turns
 * off the old LED.
 * 
 * @param nextLED The next LED to start blinking.
 * @param blinkingLED The LED blinking, used to keep track of the LED blinking.
 */
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

    // If you want to send a command to toggle the cars indicator do so here
    if (*blinkingLED != NULL)
        toggleLED((*blinkingLED)->LED_pin);
}