// Mini_Button Arduino library
// https://github.com/solamyl/Mini_Button
// Copyright (C) 2025 Štěpán Škrob, Copyright (C) 2018 by Jack Christensen
// licensed under GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html
//
// Example sketch that uses auto-repeat feature with short and long presses
// to adjust a number up and down, between two limits. Short presses increment
// or decrement by one, long presses repeat at a specified rate.
// Every time the number changes, it is written to the serial monitor.

#include <Arduino.h>

#include <Mini_Button.h>            // https://github.com/solamyl/Mini_Button

// pin assignments
const byte
    DN_PIN(7),                      // connect a button switch from this pin to ground
    UP_PIN(8);                      // ditto

const unsigned long
    REPEAT_FIRST(500),              // ms required before repeating on long press
    REPEAT_INCR(100);               // repeat interval for long press
const int
    MIN_COUNT(0),
    MAX_COUNT(59);

AutoRepeatButton                    // define the buttons
    btnUP(UP_PIN, REPEAT_FIRST, REPEAT_INCR),
    btnDN(DN_PIN, REPEAT_FIRST, REPEAT_INCR);

void setup()
{
    btnUP.begin();
    btnDN.begin();
    Serial.begin(115200);
}

void loop()
{
    static int
        count,                      // the number that is adjusted
        lastCount(-1);              // previous value of count (initialized to ensure it's different when the sketch starts)

    btnUP.read();                   // read the buttons
    btnDN.read();

    if (count != lastCount)         // print the count if it has changed
    {
        lastCount = count;
        Serial.println(count, DEC);
    }

    if (btnUP.wasPressed()) {
        ++count;                            // increment the counter
        count = min(count, MAX_COUNT);      // but not more than the specified maximum
    }
    else if (btnDN.wasPressed()) {
        --count;                            // decrement the counter
        count = max(count, MIN_COUNT);      // but not less than the specified minimum
    }
}
