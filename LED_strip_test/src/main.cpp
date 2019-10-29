// Rose Warren
// Willem Hillier

#include <Arduino.h>
#include <FastLED.h>


// The number of LEDs in the strip
#define NUM_LEDS 8

#define LED_PIN     5
#define BTN_PIN     2
#define BRIGHTNESS  64
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define UPDATES_PER_SECOND 100

#define POLICE_BLINK_DELAY 65

// Define the array of leds
CRGBArray<NUM_LEDS> leds;
uint8_t colorIndex = 0;

// Define palette and blending
CRGBPalette16 currentPalette;
TBlendType    currentBlending;

//state machine variables
int state = 0;
#define NUM_STATES 3


// Cycle through the rainbow
void hueCycle(){
  for(int baseHue = 0; baseHue < 255; baseHue ++){
    for(int led = 0; led < NUM_LEDS; led ++){
      leds[led] = CHSV(baseHue + (255/NUM_LEDS)*led, 255, 255);
    }
    FastLED.delay(20);
  }
}

// Commit a felony if attached to a motorized vehicle
void policeBlinkAlternate(){
  leds = CRGB::Black;

  // flash right red
  for(int flashes = 0; flashes < 3; flashes++){
    // red on
    for(int i = 0; i< NUM_LEDS/2; i=i+1){
      leds[i] = CRGB::Red;
    }

    FastLED.delay(POLICE_BLINK_DELAY);

    //all off
    leds = CRGB::Black;

    FastLED.delay(POLICE_BLINK_DELAY);

  }

  // flash left blue
  for(int flashes = 0; flashes < 3; flashes++){
    // blue on
    for(int i = NUM_LEDS/2; i< NUM_LEDS; i=i+1){
      leds[i] = CRGB::Blue;
    }

    FastLED.delay(POLICE_BLINK_DELAY);

    //all off
    leds = CRGB::Black;

    FastLED.delay(POLICE_BLINK_DELAY);
  }
}

// Cycle less smoothly through the rainbow
void FillLEDsFromPaletteColors(){
    uint8_t brightness = 255;

    for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }
    FastLED.delay(20);
}

// The function that actually changes the program's state when called in the interrupt
void changeState() {
  if(state < NUM_STATES - 1){
    state++;
  } else {
    state = 0;
  }
}

void setup() {
  Serial.begin(9600);
  
  // Indicating the type of LED strip used
  FastLED.addLeds<WS2812, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(  BRIGHTNESS );

  // The built-in palette used for FillLEDsFromPaletteColors
  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;

  // interrupt setup for mode-change button
  pinMode(BTN_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BTN_PIN), changeState, FALLING);
}

void loop() {

  // Debugging statements
  Serial.print("main: ");
  Serial.println(state);

  // The switches that determine which LED pattern is displayed based on state
   switch(state){
    case 0:
      FillLEDsFromPaletteColors();
      break;
    case 1:
      policeBlinkAlternate();
      break;
    case 2:
     hueCycle();
     break;
  }
}