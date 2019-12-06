#include <Arduino.h>
#include <WS2812FX.h>
#include "Button.h"
#ifdef ESP32
  #include "ESP32_RMT_Driver.h"
#endif

#define NUMBER_OF_LEDS 339
#define LED_START_BRIGHTNESS 51
#define LED_PIN D5
#define BUTTON_PIN D8
#define FADE_ON_TIME 2000
#define FADE_OFF_TIME 500
#define STEP_ON_TIME 2000
#define STEP_OFF_TIME 500
#define ANIMATION_ON_STARTUP 1 // 0: keine 1: fade on 2: step on

#define YELLOW (uint32_t)0xFF8800
#define DARKWHITE (uint32_t)0x22000000

bool stepOn(int channel, int startBit, int stopBit, long color, int time, bool onOff);
bool fadeOn(int channel, int startBit, int stopBit, long color, int time, bool onOff);
void showLEDsInFrames(unsigned int frames);
#ifdef ESP32
  void myCustomShow1(void);
#endif

WS2812FX leds = WS2812FX(NUMBER_OF_LEDS, LED_PIN, NEO_GRBW + NEO_KHZ800);
Button button(BUTTON_PIN);

bool animationRunning = false;
int animationMode = -1;
int clickStatus = ANIMATION_ON_STARTUP;
unsigned long startOfAnimationTime = 0;

void setup() {
  // Start Serial Connection
  Serial.begin(115200);
  delay(100);
  Serial.println("\nWillkommen zu den Olympischen Spielen");

  // LEDs initialisieren
  leds.init();
  #ifdef ESP32
    rmt_tx_int(RMT_CHANNEL_0, leds.getPin());
    leds.setCustomShow(myCustomShow1); 
  #endif
  leds.setBrightness(LED_START_BRIGHTNESS);
  leds.start();
}

void loop() {
  // Aktualisiere LEDs
  showLEDsInFrames(35);

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
      uint8_t tempBrightness = leds.getBrightness();
      tempBrightness += 51;
      if (tempBrightness < 51) tempBrightness = 51;
      leds.setBrightness(tempBrightness);
      Serial.printf("Helligkeit auf: %d\n", leds.getBrightness());
      break;
  }

  // Schalte Lampe abhängig vom geforderten Mode
  if (animationRunning){
    bool aR[10] = {true, true, true, true, true, true, true, true, true, true};
    switch (animationMode){
      case 1:
        aR[0] = fadeOn(0, 0, NUMBER_OF_LEDS, 0xFF000000, FADE_ON_TIME, true);
        //aR[0] = fadeOn(0, 0, NUMBER_OF_LEDS-1, WHITE, FADE_ON_TIME, true);
        break;
      case -1:
        aR[0] = fadeOn(0, 0, NUMBER_OF_LEDS, 0xFF000000, FADE_OFF_TIME, false); // RGBW
        //aR[0] = fadeOn(0, 0, NUMBER_OF_LEDS-1, WHITE, FADE_OFF_TIME, false);
        break;
      case 2:
        aR[0] = stepOn(0, 0, 39, RED, STEP_ON_TIME, true);
        aR[1] = stepOn(1, 40, 71, RED, STEP_ON_TIME, true);
        aR[2] = stepOn(2, 71, 108, GREEN, STEP_ON_TIME, true);
        aR[3] = stepOn(3, 109, 136, GREEN, STEP_ON_TIME, true);
        aR[4] = stepOn(4, 137, 173, DARKWHITE, STEP_ON_TIME, true);
        aR[5] = stepOn(5, 174, 201, DARKWHITE, STEP_ON_TIME, true);
        aR[6] = stepOn(6, 202, 238, YELLOW, STEP_ON_TIME, true);
        aR[7] = stepOn(7, 239, 266, YELLOW, STEP_ON_TIME, true);
        aR[8] = stepOn(8, 267, 306, BLUE, STEP_ON_TIME, true);
        aR[9] = stepOn(9, 307, 338, BLUE, STEP_ON_TIME, true);
        break;
      case -2:
        aR[0] = stepOn(0, 0, 39, RED, STEP_OFF_TIME, false);
        aR[1] = stepOn(1, 40, 71, RED, STEP_OFF_TIME, false);
        aR[2] = stepOn(2, 71, 108, GREEN, STEP_OFF_TIME, false);
        aR[3] = stepOn(3, 109, 136, GREEN, STEP_OFF_TIME, false);
        aR[4] = stepOn(4, 137, 173, DARKWHITE, STEP_OFF_TIME, false);
        aR[5] = stepOn(5, 174, 201, DARKWHITE, STEP_OFF_TIME, false);
        aR[6] = stepOn(6, 202, 238, YELLOW, STEP_OFF_TIME, false);
        aR[7] = stepOn(7, 239, 266, YELLOW, STEP_OFF_TIME, false);
        aR[8] = stepOn(8, 267, 306, BLUE, STEP_OFF_TIME, false);
        aR[9] = stepOn(9, 307, 338, BLUE, STEP_OFF_TIME, false);
        break;
    }
    if (aR[0] && aR[1] && aR[2] && aR[3] && aR[4] && aR[5] && aR[6] && aR[7] && aR[8] && aR[9]){  // Wenn alle Segmente true zurückgeben ist die Animation fertig
      animationRunning = false;
      Serial.printf("<%ldms>\n", millis()-startOfAnimationTime); // Debugging
    }
  }
  
  // Checke Eingabe
  clickStatus = button.ButtonLoop();
  if (Serial.available() > 0){ // Eingabe über Serial
    clickStatus = Serial.parseInt();
    Serial.println(clickStatus);
  }
}

bool stepOn(int channel, int startBit, int stopBit, long color, int time, bool onOff){
  bool animationStatus = false;
  static unsigned long lastMillis[10] = {};
  static int channelCounter[10] = {};
  unsigned int refreshTime = ((float)time/(stopBit-startBit)+0.5);
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
  unsigned int refreshTime = ((float)time/(255)+0.5);
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

void showLEDsInFrames(unsigned int frames){
  static unsigned long lastMillis = millis();
  if ( (millis()-lastMillis) >= (1000UL/frames) ){
    leds.show();
    lastMillis = millis();
  }
}

#ifdef ESP32
void myCustomShow1(void) {
  uint8_t *pixels = leds.getPixels();
  uint16_t numBytes = leds.getNumBytes() + 1;
  rmt_write_sample(RMT_CHANNEL_0, pixels, numBytes, false);
}
#endif