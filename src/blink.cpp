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

/* ------------------------------------ */
// Handle to the hardware timer
hw_timer_t* blinkTimer = NULL;

// Flags when the blinkerTimer alarm has gone off
SemaphoreHandle_t blinkSemaphore;

// Defining linkers
Linker Left_indicator(PIN_Left_Button, PIN_Left_LED);
Linker Hazard_indicator(PIN_Hazard_Button, PIN_Hazard_LED);
Linker Right_indicator(PIN_Right_Button, PIN_Right_LED);

// Keeps track of what LED to blink and what LED was being blinked
Linker* blink = NULL;
Linker* newBlink = NULL;

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

void checkIfBlink() {
    // Tries to take the semaphone and returns one if there is one to take
    if(xSemaphoreTake(blinkSemaphore, 0) == 1)
        blinker(&newBlink, &blink);
}

void init_blink() {
    // Timer setup
    blinkTimer = timerBegin(0, 80, true);
    timerAttachInterrupt(blinkTimer, &timer_isr, true);
    timerAlarmWrite(blinkTimer, BLINKER_SPEED * 1000, true);
    timerAlarmEnable(blinkTimer);

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



    // Create a binary semaphore, (Can only flag high or low)
    blinkSemaphore = xSemaphoreCreateBinary();
}

/* ------------------------------------ */