#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

class Button {
    public:
        // Constructor
        Button(int buttonPin);
        // Member Function
        int ButtonLoop();

    private:
        // Member Variables
        unsigned int debounce;          // ms debounce period to prevent flickering when pressing or releasing the button
        unsigned int DCgap;            // max ms between clicks for a double click event
        unsigned int holdTime;        // ms hold period: how long to wait for press+hold event
        unsigned int longHoldTime;    // ms long hold period: how long to wait for press+hold event
        unsigned int threshold;
        int buttonPin;

        boolean buttonVal = HIGH;   // value read from button
        boolean buttonLast = HIGH;  // buffered value of the button's previous state
        boolean DCwaiting = false;  // whether we're waiting for a double click (down)
        boolean DConUp = false;     // whether to register a double click on next release, or whether to wait and click
        boolean singleOK = true;    // whether it's OK to do a single click
        unsigned long downTime = -1;         // time the button was pressed down
        unsigned long upTime = -1;           // time the button was released
        boolean ignoreUp = false;   // whether to ignore the button release because the click+hold was triggered
        boolean waitForUp = false;        // when held, whether to wait for the up event
        boolean holdEventPast = false;    // whether or not the hold event happened already
        boolean longHoldEventPast = false;// whether or not the long hold event happened already
        unsigned int lastmillis = 0;
};
#endif