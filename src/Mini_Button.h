// Mini_Button Arduino library
// https://github.com/solamyl/Mini_Button
// Copyright (C) 2025 by Štěpán Škrob, Copyright (C) 2018 by Jack Christensen
// licensed under GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html

#ifndef MINI_BUTTON_H_INCLUDED
#define MINI_BUTTON_H_INCLUDED

#include <Arduino.h>

class Button
{
    // debounce time is limited to 60 sec
    static constexpr uint32_t m_maxDebounceTime = 60000ul;

    public:
        // Button(pin, dbTime, pullup, invert) instantiates a button object.
        //
        // Required parameter:
        // pin      The Arduino pin the button is connected to
        //
        // Optional parameters:
        // dbTime   Debounce time in milliseconds (max 60000ms, default 25ms)
        // pullup   true to enable the AVR internal pullup resistor (default true)
        // invert   true to interpret a low logic level as pressed (default true)
        Button(uint8_t pin, uint32_t dbTime=25, bool pullup=true, bool invert=true)
            : m_pin(pin), m_dbTime(dbTime), m_pullup(pullup), m_invert(invert),
            m_debouncing(false), m_state(false), m_lastState(false)
        {
            if (dbTime > m_maxDebounceTime)
                m_dbTime = m_maxDebounceTime;
        }

        // Initialize a Button object
        void begin();

        // Returns the state of the button, true if pressed, false if released.
        // Does debouncing, captures and maintains times, previous state, etc.
        // Call this function frequently to ensure the sketch is responsive to user input.
        bool read();

        // Returns true if the button state was pressed at the last call to read().
        // Does not cause the button to be read.
        bool isPressed() const {return m_state;}

        // Returns true if the button state was released at the last call to read().
        // Does not cause the button to be read.
        bool isReleased() const {return !m_state;}

        // These functions check the button state to see if it changed
        // between the last two reads and return true or false accordingly.
        // These functions do not cause the button to be read.
        bool wasPressed() const {return m_state && changed();}
        bool wasReleased() const {return !m_state && changed();}

        // Returns true if the button state at the last call to read() was pressed,
        // and has been in that state for at least the given number of milliseconds.
        // This function does not cause the button to be read.
        bool pressedFor(uint32_t ms) const {return m_state && (millis() - m_lastChange) >= ms;}

        // Returns true if the button state at the last call to read() was released,
        // and has been in that state for at least the given number of milliseconds.
        // This function does not cause the button to be read.
        bool releasedFor(uint32_t ms) const {return !m_state && (millis() - m_lastChange) >= ms;}

        // Returns the time in milliseconds (from millis) that the button last
        // changed state.
        uint32_t lastChange() const {return m_lastChange;}

        // has the state changed since the last call to the read()?
        bool changed() const {return m_state != m_lastState;}

    private:
        uint8_t m_pin;                  // arduino pin number connected to button
        uint16_t m_dbTime;              // debounce time (ms; limited to max 60000ms)
        bool m_pullup : 1;              // internal pullup resistor enabled
        bool m_invert : 1;              // if true, interpret logic low as pressed, else interpret logic high as pressed
        bool m_debouncing : 1;          // if true, we are in "debouncing" mode
        bool m_state : 1;               // current button state, true=pressed
        bool m_lastState : 1;           // button state at the previous call to the read()
        uint32_t m_lastChange {0};      // timestamp of the last state change (ms from millis)
        uint16_t m_dbStart;             // debounce interval start time (low 16-bits from millis)
};

// a derived class for a "push-on, push-off" (toggle) type button.
// initial state can be given, default is off (false).
class ToggleButton : public Button
{
    public:

        // constructor is similar to Button, but includes the initial state for the toggle.
        ToggleButton(uint8_t pin, bool initialState=false, uint32_t dbTime=25, bool pullup=true, bool invert=true)
            : Button(pin, dbTime, pullup, invert), m_toggleState(initialState) {}

        // read the button and return its state.
        // should be called frequently.
        bool read()
        {
            Button::read();
            if (wasPressed()) {
                m_toggleState = !m_toggleState;
            }
            return m_toggleState;
        }

        // has the state changed?
        bool changed() const {return wasPressed();} //the state changes just and only on wasPressed()

        // return the current state
        bool toggleState() const {return m_toggleState;}

    private:
        bool m_toggleState;
};

#endif // MINI_BUTTON_H_INCLUDED
