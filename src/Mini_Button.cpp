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
    m_time = millis();
    m_lastState = m_state;
    //m_changed = false;
    m_lastChange = m_time;
}

// returns the state of the button, true if pressed, false if released.
// does debouncing, captures and maintains times, previous state, etc.
bool Button::read()
{
    m_time = millis();
    bool pinVal = static_cast<bool>(digitalRead(m_pin)) ^ m_invert;

    m_lastState = m_state;

    switch (m_fsm) {
        case STABLE:
            if (pinVal != m_state) {    // maybe a change, but debounce first
                m_dbStart = m_time;
                m_fsm = DEBOUNCE;
            }
            //m_changed = false;
            break;

        case DEBOUNCE:
            if (m_time - m_dbStart >= m_dbTime) {
                m_fsm = STABLE;
                if (pinVal != m_state) {    // a real change (else just noise)
                    m_state = pinVal;
                    m_lastChange = m_time;
                    //m_changed = true;
                }
            }
            break;
    }
    return m_state;
}
