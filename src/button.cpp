#include "main.h"

int lastState = HIGH; // the previous state from the input pin
int currentState; // the current reading from the input pin
unsigned long pressedTime = 0;
unsigned long releasedTime = 0;

void button_setup(void)
{
    Serial.println("[BTN] setup begin");

    pinMode(BUTTON_PIN, INPUT);

    Serial.println("[BTN] setup done");
}

void button_loop(void)
{
    // read the state of the switch/button
    currentState = digitalRead(BUTTON_PIN);

    if (lastState == HIGH && currentState == LOW) {
        // button is pressed
        pressedTime = millis();
    } else if (lastState == LOW && currentState == HIGH) {
        // button is released
        releasedTime = millis();

        long pressDuration = releasedTime - pressedTime;
        Serial.printf("[BTN] pressed: %lums\r\n", pressDuration);

        if (pressDuration < SHORT_PRESS_TIME)
            Serial.println("[BTN] short press is detected");

        if (pressDuration > LONG_PRESS_TIME) {
            Serial.println("[BTN] long press is detected");
            factory_reset("btn_factoryreset");
        }
    }

    // save the the last state
    lastState = currentState;
}
