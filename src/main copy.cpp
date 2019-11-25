// #include <Arduino.h>
// #include <WS2812FX.h>
// #include "ESP32_RMT_Driver.h"
// #include "TouchButton.h"

// void myCustomShow1(void);
// bool stepOn(int channel, int startBit, int stopBit, long color, int time, bool onOff);
// bool fadeOn(int channel, int startBit, int stopBit, long color, int time, bool onOff);
// void show(int frames);

// WS2812FX ws2812fx = WS2812FX(301, 5, NEO_GRB + NEO_KHZ800);
// TouchButton touchButton(4);

// bool animationRunning = false;
// int animationMode = -1;
// int clickStatus = 0;
// unsigned long zeit = 0;
// int step = 0;

// void setup() {
//   Serial.begin(115200);

//   ws2812fx.init();
//   rmt_tx_int(RMT_CHANNEL_0, ws2812fx.getPin());
//   ws2812fx.setCustomShow(myCustomShow1);
//   ws2812fx.setBrightness(50);
//   ws2812fx.start();
// }

// void loop() {
//   //touchButton.printTouchVal(1000);
//   clickStatus = touchButton.touchButtonLoop();
//   if (Serial.available() > 0) {clickStatus = Serial.parseInt(); Serial.println(clickStatus);}
//   // else clickStatus = 0;

//   show(24);

//   switch (clickStatus){
//     case 0:
//       break;
//     case 1:
//       if ( animationRunning || ( animationMode > 0)){
//         Serial.println("Animation umkehren");
//         animationMode = -animationMode; // Animation umkehren
//       } else if ( animationMode < 0 ){
//         animationMode = 2;
//       }
//       animationRunning = true;
//       zeit = millis(); // Debugging
//       break;
//     case 2:
//       if ( animationRunning || ( animationMode > 0)){
//         Serial.println("Animation umkehren");
//         animationMode = -animationMode; // Animation umkehren
//       } else if ( animationMode < 0 ){
//         animationMode = 1;
//       }
//       animationRunning = true;
//       zeit = millis(); // Debugging
//       break;
//     case 3:
//       Serial.println("hold");
//   }

//   if (animationRunning){
//     bool aR[10] = {true, true, true, true, true, true, true, true, true, true};
//     switch (animationMode){
//       case -1:
//         aR[0] = stepOn(0, 0, 30, RED, 200, false);
//         aR[1] = stepOn(1, 31, 60, GREEN, 200, false);
//         aR[2] = stepOn(2, 61, 90, BLUE, 200, false);
//         aR[3] = stepOn(3, 91, 120, RED, 200, false);
//         aR[4] = stepOn(4, 121, 150, GREEN, 200, false);
//         aR[5] = stepOn(5, 151, 180, BLUE, 200, false);
//         aR[6] = stepOn(6, 181, 210, RED, 200, false);
//         aR[7] = stepOn(7, 211, 240, GREEN, 200, false);
//         aR[8] = stepOn(8, 241, 270, BLUE, 200, false);
//         aR[9] = stepOn(9, 271, 300, BLUE, 200, false);
//         break;
//       case 1:
//         aR[0] = stepOn(0, 0, 30, RED, 2000, true);
//         aR[1] = stepOn(1, 31, 60, GREEN, 2000, true);
//         aR[2] = stepOn(2, 61, 90, BLUE, 2000, true);
//         aR[3] = stepOn(3, 91, 120, RED, 2000, true);
//         aR[4] = stepOn(4, 121, 150, GREEN, 2000, true);
//         aR[5] = stepOn(5, 151, 180, BLUE, 2000, true);
//         aR[6] = stepOn(6, 181, 210, RED, 2000, true);
//         aR[7] = stepOn(7, 211, 240, GREEN, 2000, true);
//         aR[8] = stepOn(8, 241, 270, BLUE, 2000, true);
//         aR[9] = stepOn(9, 271, 300, BLUE, 2000, true);
//         break;
//       case -2:
//         //aR[0] = fadeOn(0, 0, 10, 0xFF000000, 200, false); // RGBW
//         aR[0] = fadeOn(0, 0, 10, WHITE, 200, false);
//         break;
//       case 2:
//         //aR[0] = fadeOn(0, 0, 10, 0xFF000000, 2000, true);
//         aR[0] = fadeOn(0, 0, 10, WHITE, 2000, true);
//         break;
//     }
//     if (aR[0] && aR[1] && aR[2] && aR[3] && aR[4] && aR[5] && aR[6] && aR[7] && aR[8] && aR[9]){
//       animationRunning = false;
//       Serial.println(millis()-zeit); // Debugging
//     }
//   }
// }

// void myCustomShow1(void) {
//   uint8_t *pixels = ws2812fx.getPixels();
//   uint16_t numBytes = ws2812fx.getNumBytes() + 1;
//   rmt_write_sample(RMT_CHANNEL_0, pixels, numBytes, false);
// }

// bool stepOn(int channel, int startBit, int stopBit, long color, int time, bool onOff){
//   bool animationStatus = false;
//   static unsigned long lastMillis[10] = {};
//   static int channelCounter[10] = {};
//   int refreshTime = ((float)time/(stopBit-startBit-1)+0.5);
//   if ( (millis()-lastMillis[channel]) >= refreshTime ){
//     if (onOff){
//       if (channelCounter[channel] <= (stopBit - startBit)){
//         ws2812fx.setPixelColor(startBit+channelCounter[channel], color);
//         channelCounter[channel]++;          
//         lastMillis[channel] = millis();
//       }else animationStatus = true;
//     }else{
//       if (channelCounter[channel] > 0 ){
//         ws2812fx.setPixelColor(startBit+channelCounter[channel]-1, BLACK);
//         channelCounter[channel]--;          
//         lastMillis[channel] = millis();
//       }else animationStatus = true;
//     }
//   } else{
//     if (onOff){
//       if (channelCounter[channel] > (stopBit - startBit))
//         animationStatus = true;
//     }else{
//       if (channelCounter[channel] <= 0 )
//         animationStatus = true;
//     }
//   }
//   return animationStatus;
// }

// bool fadeOn(int channel, int startBit, int stopBit, long color, int time, bool onOff){
//   bool animationStatus = false;
//   static unsigned lastMillis[10] = {};
//   static int channelCounter[10] = {};
//   int refreshTime = ((float)time/254+0.5);
//   int r, g, b, w;
//   w = color >> 24 & 0xFF; r = color >> 16 & 0xFF; g = color >> 8 & 0xFF; b = color & 0xFF;
//   if ( (millis()-lastMillis[channel]) >= refreshTime) {
//     if (onOff){
//       if (channelCounter[channel] <= 255){ 
//         ws2812fx.fill(ws2812fx.Color(r/255*(channelCounter[channel]), g/255*(channelCounter[channel]), b/255*(channelCounter[channel]), w/255*(channelCounter[channel])), startBit, stopBit);
//         channelCounter[channel]++;
//         lastMillis[channel] = millis();
//       }else animationStatus = true;
//     } else{
//       if (channelCounter[channel] > 0 ){
//         ws2812fx.fill(ws2812fx.Color(r/255*(channelCounter[channel]-1), g/255*(channelCounter[channel]-1), b/255*(channelCounter[channel]-1), w/255*(channelCounter[channel]-1)), startBit, stopBit);
//         channelCounter[channel]--; 
//         lastMillis[channel] = millis();
//       }else animationStatus = true;
//     }
//   } else{
//     if (onOff){
//       if (channelCounter[channel] > 255)
//         animationStatus = true;
//     }else{
//       if (channelCounter[channel] <= 0 )
//         animationStatus = true;
//     }
//   }
//   return animationStatus;
// }

// void show(int frames){
//   static unsigned long lastMillis = millis();
//   if ( (millis()-lastMillis) >= (1000UL/frames) ){
//     ws2812fx.show();
//     lastMillis = millis();
//   }
// }

