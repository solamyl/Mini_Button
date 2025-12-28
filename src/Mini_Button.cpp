// Mini_Button Arduino library
// https://github.com/solamyl/Mini_Button
// Copyright (C) 2025 by Štěpán Škrob, Copyright (C) 2018 by Jack Christensen
// licensed under GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html

#include "Mini_Button.h"

// initialize a Button object
void Button::begin()
{
    pinMode(m_pin, m_pullup ? INPUT_PULLUP : INPUT);
    m_state = static_cast<bool>(digitalRead(m_pin)) ^ m_invert;
    m_lastState = m_state;
    m_lastChange = millis();
}

// returns the state of the button, true if pressed, false if released.
// does debouncing, captures and maintains times, previous state, etc.
bool Button::read()
{
    uint32_t now = millis();
    uint16_t now16 = static_cast<uint16_t>(now); // low 16-bits from millis()
    bool pinVal = static_cast<bool>(digitalRead(m_pin)) ^ m_invert;

    m_lastState = m_state;

    if (m_debouncing) {
        // "debouncing" mode
        if (pinVal != m_state) {
            // pinVal still has the changed value - continue with the debouncing
            if (now16 - m_dbStart >= m_dbTime) {
                // pinVal is stable long enough => change the state
                m_state = pinVal;
                m_lastChange = now;
                m_debouncing = false; // stop debouncing
            }
        }
        else {
            // pinVal returned back to a previous state
            m_debouncing = false; // end debouncing
        }
    }
    else {
        // "stable state" mode
        if (pinVal != m_state) {
            // change on input => start debouncing
            m_dbStart = now16;
            m_debouncing = true;
        }
        else {
            // nothing happens
        }
    }
    return m_state;
}

// read the button and return its state.
// should be called frequently.
bool ToggleButton::read()
{
    Button::read(); //read input and do the debouncing

    if (Button::wasPressed()) {
        m_toggleState = !m_toggleState;
    }
    return m_toggleState;
}

#if 0
// read the button, obtain physical state and then calculate virtual state
// repeating is made by inserting fake key-releases into the stream of "pressed" states
// should be called frequently
bool AutoRepeatButton::read()
{
    Button::read(); //read input and do the debouncing

    m_virtualLastState = m_virtualState;
    m_virtualState = Button::isPressed();

    if (m_virtualState) {
        uint32_t t = millis() - lastChange();
        if (t >= m_delay) { //skip the init delay
            // repeating...
            t -= m_delay;
            t %= m_rate; //cycle in m_rate periods
            if (t < debounceTime()) //if start of the period
                m_virtualState = false; //make a virtual key release
        }
    }
    return m_virtualState;
}

#else

// alternative implementation, previous may not work correctly for slower loop()
bool AutoRepeatButton::read()
{
    Button::read(); //read input and do the debouncing

    m_virtualLastState = m_virtualState;
    m_virtualState = Button::isPressed();

    if (m_virtualState) {
        // someone is holding the button
        uint32_t t = millis() - lastChange(); //how long?
        if (t >= m_delay) {
            // do the repeating after m_delay has elapsed
            t -= m_delay;
            uint16_t cnt = t / m_rate + 1; //what repeat cycle # is it now?
            if (m_repeatCounter < cnt && m_virtualLastState) {
                // if time for next key-release AND lastState=="pressed"
                m_virtualState = false; //make a virtual key release
                m_repeatCounter++;
            }
        }
    }
    else {
        // physical button is released
        m_repeatCounter = 0;
    }
    return m_virtualState;
}
#endif


