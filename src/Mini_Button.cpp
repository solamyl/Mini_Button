// Mini_Button Arduino library
// https://github.com/solamyl/Mini_Button
// Copyright (C) 2025 by Štěpán Škrob, Copyright (C) 2018 by Jack Christensen
// licensed under GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html

#include "Mini_Button.h"

// initialize a Button object
void Button::begin()
{
    pinMode(m_pin, m_puEnable ? INPUT_PULLUP : INPUT);
    m_state = static_cast<bool>(digitalRead(m_pin)) ^ m_invert;
    m_lastState = m_state;
    m_time = millis();
    m_lastChange = m_time;
}

// returns the state of the button, true if pressed, false if released.
// does debouncing, captures and maintains times, previous state, etc.
bool Button::read()
{
    m_time = millis();
    bool pinVal = static_cast<bool>(digitalRead(m_pin)) ^ m_invert;

    m_lastState = m_state;

    if (m_debouncing) {
        // "debouncing" mode
        if (pinVal != m_state) {
            // pinVal still has the changed value - continue with the debouncing
            uint16_t m_time16 = static_cast<uint16_t>(m_time);
            if (m_time16 - m_dbStart >= m_dbTime) {
                // pinVal is stable long enough => change the state
                m_state = pinVal;
                m_lastChange = m_time;
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
            m_dbStart = static_cast<uint16_t>(m_time);
            m_debouncing = true;
        }
        else {
            // nothing happens
        }
    }
    return m_state;
}
