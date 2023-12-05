#include <FastLED.h>
#include <Ramp.h>
#include <OneButton.h>

// Pin definitions
#define LED_PIN 5
#define BTN_PIN 7

// Button Initialization
OneButton btn = OneButton(BTN_PIN, true, true);

// ################## matrix ###################
const uint8_t kMatrixWidth  = 10;
const uint8_t kMatrixHeight = 11;

#define NUM_LEDS kMatrixWidth * kMatrixHeight
CRGB leds[NUM_LEDS];

boolean coil = true;
boolean flip = false;
boolean ser_col = true;
boolean prototyping = false;
boolean reporting = false;
boolean pressed = false;
boolean twinkle = false;

// ################## config ###################
uint8_t hurry = 6;
boolean dataSmoothing = true;

boolean rolling = false;
boolean triple = false;
boolean indexDrift = false;

// three brightness values to choose via button
uint8_t Bri1 = 255;
uint8_t Bri2 = 86;
uint8_t Bri3 = 86;

// prameters for initial palette selection
uint8_t base_hue1 = 30;  // first hue
uint8_t base_hue2 = 37; // second hue
uint8_t base_hue3 = base_hue2; // second hue
uint8_t range = 5;       // fluctuation

// parameter for moving the lit area
uint16_t lower = 0;        // lower end of lights
uint16_t upper = NUM_LEDS - 4; // upper end of lights
uint16_t store, up_speed, lo_speed, bri_speed;

// ################## inits ###################
// booleans for conditional execution
boolean palette_changed = false;
boolean new_colors = false;

uint8_t paletteIndex, CurrentBri, TargetBri, coin;
int speed1, speed2; // initial smoothing speeds set (0.5 seconds)
uint32_t xyVals[4];
int noiRampMin[4];
int noiRampMax[4];
int rampStore[4];

CRGB col[4]; // these are the currently used colors
CRGB pal[4]; // these are colors of a new palette that we blend to

// initializing smoothing functions
ramp briRamp, palRamp1, palRamp2; // smooth palette blending 1
ramp lowerRamp, upperRamp; // smooth area blending 1
rampLong lumRampX, lumRampY, colRampX, colRampY; // smooth luminance scale blending on X

// #############################################
// ################## SETUP ####################
void setup() {
  
  delay(1000); // startup safety delay
  Serial.begin(115200);
  randomSeed(analogRead(0));

  FastLED.addLeds < WS2812B, LED_PIN, GRB > (leds, NUM_LEDS);
  FastLED.setBrightness(0);
  FastLED.setCorrection(TypicalPixelString);

  btn.attachClick(brightnessAreaButton);
  btn.attachLongPressStart(paletteButton);
  btn.setPressMs(250);

  lumRampX.go(500, 0, LINEAR);
  lumRampY.go(500, 0, LINEAR);
  colRampX.go(500, 0, LINEAR);
  colRampY.go(500, 0, LINEAR);

  // random xy values for the noise field to ensure different starting points
  for (int i = 0; i < 4; i++)
  {
    noiRampMin[i] = 5000;
    noiRampMax[i] = 15000;
    xyVals[i]     = random(10000);
  }

  changeScales(10000);
  
  buildPalette(range, true, false);
  for (uint8_t i = 0; i < 4; i++)
  { col[i] = pal[i];}

  bri_speed = 4500; lo_speed = 5000; up_speed = 5000;
  TargetBri = Bri1;
  brightnessAreaButton();

  Serial.println("Hello Lamp");
}

// #############################################
// ################## LOOP #####################
void loop() {

  buttonSwitches(); // control button activity
  rollColors();

  makeNoise();

  EVERY_N_MILLISECONDS(50)
  {
    if (indexDrift == true)
    {
        paletteIndex ++;
    }
  }

  EVERY_N_SECONDS(55)
  {
      changeScales(20000);
  }

  EVERY_N_SECONDS(27)
  {
    if (palRamp2.isFinished() == 1 && palette_changed == false)
    {
      buildPalette(random(0, 10), false, triple);
      triggerRoll(15000);
    }
  }

  moveRange(lowerRamp.update(), upperRamp.update(), 8);
  fadeToBlackBy(leds, NUM_LEDS, 64);
  FastLED.setBrightness(CurrentBri);
  FastLED.show();
  btn.tick();
}