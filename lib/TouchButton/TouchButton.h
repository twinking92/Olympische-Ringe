#ifndef TOUCHBUTTON_H
#define TOUCHBUTTON_H

#include <MedianFilterLib.h>

class TouchButton {
    public:
        // Constructor
        TouchButton(int touchPin);
        TouchButton(int touchPin, int debounce, int DCgap, int holdTime, int longHoldTime, int threshold);
        // Member Function
        int touchButtonLoop();
        void printTouchVal(int time);
        void setThreshold(int threshold);

    private:
        // Member Variables
        int debounce;          // ms debounce period to prevent flickering when pressing or releasing the button
        int DCgap;            // max ms between clicks for a double click event
        int holdTime;        // ms hold period: how long to wait for press+hold event
        int longHoldTime;    // ms long hold period: how long to wait for press+hold event
        int threshold;
        int touchPin;

        MedianFilter<int> filter;
        int touchVal = 0;
        int event = 0;
        boolean buttonVal = HIGH;   // value read from button
        boolean buttonLast = HIGH;  // buffered value of the button's previous state
        boolean DCwaiting = false;  // whether we're waiting for a double click (down)
        boolean DConUp = false;     // whether to register a double click on next release, or whether to wait and click
        boolean singleOK = true;    // whether it's OK to do a single click
        long downTime = -1;         // time the button was pressed down
        long upTime = -1;           // time the button was released
        boolean ignoreUp = false;   // whether to ignore the button release because the click+hold was triggered
        boolean waitForUp = false;        // when held, whether to wait for the up event
        boolean holdEventPast = false;    // whether or not the hold event happened already
        boolean longHoldEventPast = false;// whether or not the long hold event happened already
        int lastmillis = 0;
};
#endif