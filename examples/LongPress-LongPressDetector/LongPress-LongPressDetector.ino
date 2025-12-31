// Mini_Button Arduino library
// https://github.com/solamyl/Mini_Button
// Copyright (C) 2025 by Štěpán Škrob, Copyright (C) 2018 by Jack Christensen
// licensed under GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html
//
// Example sketch demonstrating short and long button presses.
//
// A simple state machine where a short press of the button turns the
// Arduino pin 13 LED on or off, and a long press causes the LED to
// blink rapidly. Once in rapid blink mode, another long press goes
// back to on/off mode.

#include <Mini_Button.h>          // https://github.com/solamyl/Mini_Button

// pin assignments
const byte
    BUTTON_PIN(7),              // connect a button switch from this pin to ground
    LED_PIN(13);                // the standard Arduino "pin 13" LED

const unsigned long
    LONG_PRESS(1000),           // we define a "long press" to be 1000 milliseconds.
    BLINK_INTERVAL(100);        // in the BLINK state, switch the LED every 100 milliseconds.

Button myBtn(BUTTON_PIN);       // define the button
LongPressDetector detectLP(&myBtn, LONG_PRESS);

void setup()
{
    myBtn.begin();              // initialize the button object
    detectLP.begin();
    pinMode(LED_PIN, OUTPUT);   // set the LED pin as an output
}

// the list of possible states for the state machine. This state machine has a fixed
// sequence of states, i.e. ONOFF --> BLINK --> ONOFF
enum states_t {ONOFF, BLINK};

bool ledState;                  // current LED status
unsigned long ms;               // current time from millis()
unsigned long msLast;           // last time the LED was switched

void loop()
{
    static states_t STATE;      // current state machine state
    ms = millis();              // record the current time
    myBtn.read();               // read the button
    detectLP.read();            // check the button state for a long press

    switch (STATE)
    {
        // this state watches for short and long presses, switches the LED for
        // short presses, and moves to the BLINK state for long presses.
        case ONOFF:
            if (detectLP.wasPressed())
                STATE = BLINK;
            // distinguish between short-press release and long-press release
            // LongPressDetector observes the same button so when released
            // then both `myBtn` and `detectLP` indicate a key release
            else if (myBtn.wasReleased() && !detectLP.wasReleased())
                switchLED();
            break;

        // the fast-blink state. Watch for another long press which will cause us to
        // turn the LED off (as feedback to the user) and move to the ONOFF state.
        case BLINK:
            if (detectLP.wasPressed())
            {
                STATE = ONOFF;
                digitalWrite(LED_PIN, LOW);
                ledState = false;
            }
            else
                fastBlink();
            break;
    }
}

// reverse the current LED state. if it's on, turn it off. if it's off, turn it on.
void switchLED()
{
    msLast = ms;                // record the last switch time
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
}

// switch the LED on and off every BLINK_INTERVAL milliseconds.
void fastBlink()
{
    if (ms - msLast >= BLINK_INTERVAL)
        switchLED();
}

