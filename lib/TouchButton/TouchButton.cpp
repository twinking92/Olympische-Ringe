//=================================================
//  MULTI-CLICK:  One Button, Multiple Events

#include <TouchButton.h>

TouchButton::TouchButton(int touchPin):debounce(50), DCgap(250), holdTime(2000), longHoldTime(3000), threshold(55), touchPin(touchPin), filter(5){}
TouchButton::TouchButton(int touchPin, int debounce, int DCgap, int holdTime, int longHoldTime, int threshold):debounce(debounce), DCgap(DCgap), holdTime(holdTime), longHoldTime(longHoldTime), threshold(threshold), touchPin(touchPin), filter(5){}

int TouchButton::touchButtonLoop(){    
   event = 0;
   touchVal = 0;
   touchVal = filter.AddValue(touchRead(touchPin));
   if (touchVal < threshold){
        buttonVal = LOW;
   } else {buttonVal= HIGH;}
   // Button pressed down
   if (buttonVal == LOW && buttonLast == HIGH && (millis() - upTime) > debounce)
   {
       downTime = millis();
       ignoreUp = false;
       waitForUp = false;
       singleOK = true;
       holdEventPast = false;
       longHoldEventPast = false;
       if ((millis()-upTime) < DCgap && DConUp == false && DCwaiting == true)  DConUp = true;
       else  DConUp = false;
       DCwaiting = false;
   }
   // Button released
   else if (buttonVal == HIGH && buttonLast == LOW && (millis() - downTime) > debounce)
   {        
       if (not ignoreUp)
       {
           upTime = millis();
           if (DConUp == false) DCwaiting = true;
           else
           {
               event = 2;
               DConUp = false;
               DCwaiting = false;
               singleOK = false;
           }
       }
   }
   // Test for normal click event: DCgap expired
   if ( buttonVal == HIGH && (millis()-upTime) >= DCgap && DCwaiting == true && DConUp == false && singleOK == true && event != 2)
   {
       event = 1;
       DCwaiting = false;
   }
   // Test for hold
   if (buttonVal == LOW && (millis() - downTime) >= holdTime) {
       // Trigger "normal" hold
       if (not holdEventPast)
       {
           event = 3;
           waitForUp = true;
           ignoreUp = true;
           DConUp = false;
           DCwaiting = false;
           //downTime = millis();
           holdEventPast = true;
       }
       // Trigger "long" hold
       if ((millis() - downTime) >= longHoldTime)
       {
           if (not longHoldEventPast)
           {
               event = 4;
               longHoldEventPast = true;
           }
       }
   }
   buttonLast = buttonVal;
   return event;
}

void TouchButton::printTouchVal(int time){
    if (millis()-lastmillis > time){
        Serial.printf("Current touch value is: %d.\n\n", touchVal);
        lastmillis = millis();
    }
}

void TouchButton::setThreshold(int threshold){
    this->threshold = threshold;
}