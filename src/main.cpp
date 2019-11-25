#include <Arduino.h>
#include <WS2812FX.h>
#include "ESP32_RMT_Driver.h"
#include "TouchButton.h"

#define NUMBER_OF_LEDS 300
#define LED_MAX_BRIGHTNESS 10
#define LED_PIN 5
#define TOUCH_PIN 4
#define FADE_ON_TIME 2000
#define FADE_OFF_TIME 500
#define STEP_ON_TIME 2000
#define STEP_OFF_TIME 500

void myCustomShow1(void);
bool stepOn(int channel, int startBit, int stopBit, long color, int time, bool onOff);
bool fadeOn(int channel, int startBit, int stopBit, long color, int time, bool onOff);
void showLEDsInFrames(int frames);

WS2812FX leds = WS2812FX(NUMBER_OF_LEDS, LED_PIN, NEO_GRBW + NEO_KHZ800);
TouchButton touchButton(TOUCH_PIN);

bool animationRunning = false;
int animationMode = -1;
int clickStatus = 0;
unsigned long startOfAnimationTime = 0;

void setup() {
  // Start Serial Connection
  Serial.begin(115200);
  delay(100);
  Serial.println("\nWillkommen zu den Olympischen Spielen");

  // LEDs initialisieren
  leds.init();
  rmt_tx_int(RMT_CHANNEL_0, leds.getPin());
  leds.setCustomShow(myCustomShow1);
  leds.setBrightness(LED_MAX_BRIGHTNESS);
  leds.start();
}

void loop() {
  // Aktualisiere LEDs
  showLEDsInFrames(24);
  
  // Checke Eingabe
  //touchButton.printTouchVal(10);
  clickStatus = touchButton.touchButtonLoop();
  if (Serial.available() > 0){ // Eingabe über Serial
    clickStatus = Serial.parseInt();
    Serial.println(clickStatus);
  }

  // Verarbeite Eingabe
  switch (clickStatus){
    case 0:
      break; // tue nichts
    case 1:
      if ( animationRunning || ( animationMode > 0)){ // Wenn Annimation läuft und Lampe eingeschalten ist
        animationMode = -animationMode;               // -> Animation umkehren
        Serial.println("<--- Off");  
      } else if ( animationMode < 0 ){                // sonst wenn Lampe aus
        animationMode = 1;                            // -> Schalte Lampe mit Mode 1 an
        Serial.println("---> Fade On"); 
      }
      animationRunning = true;
      startOfAnimationTime = millis(); // Debugging
      break;
    case 2:
      if ( animationRunning || ( animationMode > 0)){ // Wenn Annimation läuft und Lampe eingeschalten ist
        animationMode = -animationMode;               // -> Animation umkehren
        Serial.println("<---"); 
      } else if ( animationMode < 0 ){                // sonst wenn Lampe aus
        animationMode = 2;                            // -> Schalte Lampe mit Mode 2 an
        Serial.println("---> Step On"); 
      }
      animationRunning = true;
      startOfAnimationTime = millis(); // Debugging
      break;
    case 3:
      Serial.println("hold");
      break;
  }

  // Schalte Lampe abhängig vom geforderten Mode
  if (animationRunning){
    bool aR[10] = {true, true, true, true, true, true, true, true, true, true};
    switch (animationMode){
      case 1:
        aR[0] = fadeOn(0, 0, NUMBER_OF_LEDS-1, 0xFF000000, FADE_ON_TIME, true);
        //aR[0] = fadeOn(0, 0, NUMBER_OF_LEDS-1, WHITE, FADE_ON_TIME, true);
        break;
      case -1:
        aR[0] = fadeOn(0, 0, NUMBER_OF_LEDS-1, 0xFF000000, FADE_OFF_TIME, false); // RGBW
        //aR[0] = fadeOn(0, 0, NUMBER_OF_LEDS-1, WHITE, FADE_OFF_TIME, false);
        break;
      case 2:
        aR[0] = stepOn(0, 0, 30, RED, STEP_ON_TIME, true);
        aR[1] = stepOn(1, 31, 60, GREEN, STEP_ON_TIME, true);
        aR[2] = stepOn(2, 61, 90, BLUE, STEP_ON_TIME, true);
        aR[3] = stepOn(3, 91, 120, RED, STEP_ON_TIME, true);
        aR[4] = stepOn(4, 121, 150, GREEN, STEP_ON_TIME, true);
        aR[5] = stepOn(5, 151, 180, BLUE, STEP_ON_TIME, true);
        aR[6] = stepOn(6, 181, 210, RED, STEP_ON_TIME, true);
        aR[7] = stepOn(7, 211, 240, GREEN, STEP_ON_TIME, true);
        aR[8] = stepOn(8, 241, 270, BLUE, STEP_ON_TIME, true);
        aR[9] = stepOn(9, 271, 300, BLUE, STEP_ON_TIME, true);
        break;
      case -2:
        aR[0] = stepOn(0, 0, 30, RED, STEP_OFF_TIME, false);
        aR[1] = stepOn(1, 31, 60, GREEN, STEP_OFF_TIME, false);
        aR[2] = stepOn(2, 61, 90, BLUE, STEP_OFF_TIME, false);
        aR[3] = stepOn(3, 91, 120, RED, STEP_OFF_TIME, false);
        aR[4] = stepOn(4, 121, 150, GREEN, STEP_OFF_TIME, false);
        aR[5] = stepOn(5, 151, 180, BLUE, STEP_OFF_TIME, false);
        aR[6] = stepOn(6, 181, 210, RED, STEP_OFF_TIME, false);
        aR[7] = stepOn(7, 211, 240, GREEN, STEP_OFF_TIME, false);
        aR[8] = stepOn(8, 241, 270, BLUE, STEP_OFF_TIME, false);
        aR[9] = stepOn(9, 271, 300, BLUE, STEP_OFF_TIME, false);
        break;
    }
    if (aR[0] && aR[1] && aR[2] && aR[3] && aR[4] && aR[5] && aR[6] && aR[7] && aR[8] && aR[9]){  // Wenn alle Segmente true zurückgeben ist die Animation fertig
      animationRunning = false;
      Serial.printf("<%ldms>\n", millis()-startOfAnimationTime); // Debugging
    }
  }
}

void myCustomShow1(void) {
  uint8_t *pixels = leds.getPixels();
  uint16_t numBytes = leds.getNumBytes() + 1;
  rmt_write_sample(RMT_CHANNEL_0, pixels, numBytes, false);
}

bool stepOn(int channel, int startBit, int stopBit, long color, int time, bool onOff){
  bool animationStatus = false;
  static unsigned long lastMillis[10] = {};
  static int channelCounter[10] = {};
  int refreshTime = ((float)time/(stopBit-startBit)+0.5);
  if (onOff){
    if (channelCounter[channel] <= (stopBit - startBit)){      
      if ( (millis()-lastMillis[channel]) >= refreshTime ){
        leds.setPixelColor(startBit+channelCounter[channel], color);
        channelCounter[channel]++;          
        lastMillis[channel] = millis();
      }
    }else animationStatus = true;
  }else{
    if (channelCounter[channel] > 0 ){
      if ( (millis()-lastMillis[channel]) >= refreshTime ){
        leds.setPixelColor(startBit+channelCounter[channel]-1, BLACK);
        channelCounter[channel]--;          
        lastMillis[channel] = millis();
      }
    }else animationStatus = true;
  }
  return animationStatus;
}

bool fadeOn(int channel, int startBit, int stopBit, long color, int time, bool onOff){
  bool animationStatus = false;
  static unsigned lastMillis[10] = {};
  static int channelCounter[10] = {};
  int refreshTime = ((float)time/(255)+0.5);
  int r, g, b, w;
  w = color >> 24 & 0xFF; r = color >> 16 & 0xFF; g = color >> 8 & 0xFF; b = color & 0xFF;
  if (onOff){
    if (channelCounter[channel] <= 255){ 
      if ( (millis()-lastMillis[channel]) >= refreshTime) {
        leds.fill(leds.Color(r/255*(channelCounter[channel]), g/255*(channelCounter[channel]), b/255*(channelCounter[channel]), w/255*(channelCounter[channel])), startBit, stopBit);
        channelCounter[channel]++;
        lastMillis[channel] = millis();
      }
    }else animationStatus = true;
  } else{
    if (channelCounter[channel] > 0 ){
      if ( (millis()-lastMillis[channel]) >= refreshTime) {
        leds.fill(leds.Color(r/255*(channelCounter[channel]-1), g/255*(channelCounter[channel]-1), b/255*(channelCounter[channel]-1), w/255*(channelCounter[channel]-1)), startBit, stopBit);
        channelCounter[channel]--; 
        lastMillis[channel] = millis();
      }
    }else animationStatus = true;
  }
  return animationStatus;
}

void showLEDsInFrames(int frames){
  static unsigned long lastMillis = millis();
  if ( (millis()-lastMillis) >= (1000UL/frames) ){
    leds.show();
    lastMillis = millis();
  }
}