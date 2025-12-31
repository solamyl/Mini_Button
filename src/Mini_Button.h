// Mini_Button Arduino library
// https://github.com/solamyl/Mini_Button
// Copyright (C) 2025 by Štěpán Škrob, Copyright (C) 2018 by Jack Christensen
// licensed under GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html

#ifndef MINI_BUTTON_H_INCLUDED
#define MINI_BUTTON_H_INCLUDED

#include <Arduino.h>

class Button
{
    // debounce time is limited to max 60 sec
    static constexpr uint32_t MAX_DEBOUNCE_TIME = 60000ul;

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
        Button(uint8_t pin, uint16_t dbTime=25, bool pullup=true, bool invert=true)
            : m_pin(pin), m_dbTime(dbTime), m_pullup(pullup), m_invert(invert),
            m_debouncing(false), m_state(false), m_lastState(false)
        {
            if (dbTime < 1)
                m_dbTime = 1;
            else if (dbTime > MAX_DEBOUNCE_TIME)
                m_dbTime = MAX_DEBOUNCE_TIME;
        }

        // Initialize a Button object
        void begin();

        // Returns the state of the button, true if pressed, false if released.
        // Does debouncing, captures and maintains times, previous state, etc.
        // Call this function frequently to ensure the sketch is responsive to user input.
        bool read();

        // Returns true if the button state was "pressed" at the last call to read().
        // Does not cause the button to be read.
        bool isPressed() const {return m_state;}

        // Returns true if the button state was "released" at the last call to read().
        // Does not cause the button to be read.
        bool isReleased() const {return !m_state;}

        // These functions check the button state to see if it changed
        // between the last two reads and return true or false accordingly.
        // These functions do not cause the button to be read.
        bool wasPressed() const {return isPressed() && changed();}
        bool wasReleased() const {return isReleased() && changed();}

        // Returns true if the button state at the last call to read() was pressed,
        // and has been in that state for at least the given number of milliseconds.
        // This function does not cause the button to be read.
        bool pressedFor(uint32_t ms) const {return isPressed() && (millis() - m_lastChange) >= ms;}

        // Returns true if the button state at the last call to read() was released,
        // and has been in that state for at least the given number of milliseconds.
        // This function does not cause the button to be read.
        bool releasedFor(uint32_t ms) const {return isReleased() && (millis() - m_lastChange) >= ms;}

        // Returns the time (from millis) of the last moment
        // the button has changed its "stable" state.
        uint32_t lastChange() const {return m_lastChange;}

        // has the state changed since the last call to the read()?
        bool changed() const {return m_state != m_lastState;}

    private:
        uint8_t m_pin;                  // arduino pin number connected to button
        uint16_t m_dbTime;              // debounce time (in msec; limited to max 60000ms)
        bool m_pullup : 1;              // internal pullup resistor, true=enabled
        bool m_invert : 1;              // input signal logic: true=LOW is "pressed", false=HIGH is "pressed"
        bool m_debouncing : 1;          // if true, we are in "debouncing" mode
        bool m_state : 1;               // current button state, true="pressed"
        bool m_lastState : 1;           // button state at the previous call to the read()
        uint32_t m_lastChange {0};      // timestamp of the last state change (msec from millis)
        uint16_t m_dbStart;             // debounce interval start time (low 16-bits from millis)
};

// a derived class for a "push-on, push-off" (toggle) type button.
// initial state can be given, default is off (false).
class ToggleButton : public Button
{
    public:

        // constructor is similar to Button, but includes the initial state for the toggle.
        ToggleButton(uint8_t pin, bool initState=false, uint16_t dbTime=25, bool pullup=true, bool invert=true)
            : Button(pin, dbTime, pullup, invert), m_toggleState(initState) {}

        // Initialize a button object
        void begin();

        // read the button and return its toggle-state.
        // should be called frequently.
        bool read();

        // has the state changed?
        bool changed() const {return wasPressed();} //the state changes just and only on wasPressed()

        // return the current state
        bool toggleState() const {return m_toggleState;}

    private:
        bool m_toggleState;             // flip-flop state of the switch
        uint32_t m_lastChange {0};      // timestamp of the last state change (from millis)
};


// a derived class for the button with auto-repeat feature
class AutoRepeatButton : public Button
{
    public:
        AutoRepeatButton(uint8_t pin, uint16_t delay=500, uint16_t rate=100, uint16_t dbTime=25, bool pullup=true, bool invert=true)
            : Button(pin, dbTime, pullup, invert), m_delay(delay), m_rate(rate)
        {
            if (m_delay < 1)
                m_delay = 1;
            if (m_rate < 1)
                m_rate = 1;
        }
        
        // Initialize a button object
        void begin();

        // read the button, obtain physical state and then calculate virtual state.
        // repeating is made by inserting fake key-releases into the stream of "pressed" states
        // should be called frequently
        bool read();

        // Returns true if the button state was pressed at the last call to read().
        // Does not cause the button to be read.
        bool isPressed() const {return m_virtualState;}

        // Returns true if the button state was released at the last call to read().
        // Does not cause the button to be read.
        bool isReleased() const {return !m_virtualState;}

        // These functions check the button state to see if it changed
        // between the last two reads and return true or false accordingly.
        // These functions do not cause the button to be read.
        bool wasPressed() const {return isPressed() && changed();}
        bool wasReleased() const {return isReleased() && changed();}

        // has the state changed since the last call to the read()?
        bool changed() const {return m_virtualState != m_virtualLastState;}

    private:
        uint16_t m_delay;               // delay in msec after which repeating keypresses begin
        uint16_t m_rate;                // repeating rate period in msec
        bool m_virtualState : 1;        // current state of the repeating button
        bool m_virtualLastState : 1;    // state of the repeating button at the previous call to read()
        uint16_t m_repeatCounter {0};   // number of keyreleases made when autorepeating
        uint32_t m_lastChange {0};      // timestamp of the last state change (from millis)
};


// detect a long press
class LongPressDetector{
    public:
        LongPressDetector(Button * button, uint16_t delay)
            : m_buttonPtr(button), m_delay(delay)
        {
            if (m_delay < 1)
                m_delay = 1;
        }

        // Initialize a detector object
        void begin();

        // Observes specified button for long press and then updates its state.
        // Does not any physical button input reading! Just observes state of another button!
        // Call this function frequently to ensure the sketch is responsive to user input.
        bool read();

        // Returns true if the button state was "pressed" at the last call to read().
        // Does not cause the button to be read.
        bool isPressed() const {return m_state;}

        // Returns true if the button state was "released" at the last call to read().
        // Does not cause the button to be read.
        bool isReleased() const {return !m_state;}

        // These functions check the button state to see if it changed
        // between the last two reads and return true or false accordingly.
        // These functions do not cause the button to be read.
        bool wasPressed() const {return isPressed() && changed();}
        bool wasReleased() const {return isReleased() && changed();}

        // Returns true if the button state at the last call to read() was pressed,
        // and has been in that state for at least the given number of milliseconds.
        // This function does not cause the button to be read.
        bool pressedFor(uint32_t ms) const {return m_buttonPtr->pressedFor(ms);}

        // Returns true if the button state at the last call to read() was released,
        // and has been in that state for at least the given number of milliseconds.
        // This function does not cause the button to be read.
        bool releasedFor(uint32_t ms) const {return m_buttonPtr->releasedFor(ms);}

        // Returns the time (from millis) of the last moment
        // the button has changed its "stable" state.
        uint32_t lastChange() const {return m_lastChange;}

        // has the state changed since the last call to the read()?
        bool changed() const {return m_state != m_lastState;}


    private:
        Button * m_buttonPtr;           // pointer to the button object to observe
        uint16_t m_delay;               // lenght of the press to detect
        bool m_state : 1;               // current state of this detector, true="long press"
        bool m_lastState : 1;           // state of this detector at the previous call to read()
        uint32_t m_lastChange {0};      // timestamp of the last state change (from millis)
};

#endif // MINI_BUTTON_H_INCLUDED
