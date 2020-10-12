#include <FastLED.h>

#define LED_PIN     6
#define COLOR_ORDER GRB
#define CHIPSET     WS2812
#define NUM_LEDS    64

#define BRIGHTNESS 50

CRGB leds[NUM_LEDS];

bool needsToBeReset;
int targetShape;
long blowUpTime;

bool doClearMistake;
long lastMistakeTime;

void setup() {
  delay(2000);
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness( BRIGHTNESS );
  
  Serial.begin(9600);
  delay(1000);
  needsToBeReset = false;
  doClearMistake = false;
  blowUpTime = 0;
  lastMistakeTime = 0;
  targetShape = random(3, 6);
  drawTargetShape();
}

void loop() {
  int readShape = Serial.read();
  if (readShape >= 0) {
    Serial.println(readShape);
    if (readShape == targetShape) {
      blowUp(intToColor(targetShape));
      needsToBeReset = true;
      blowUpTime = millis();
    }
    else if (readShape != 1) {
      drawMistake();
    }
  }

  if (needsToBeReset && millis() - blowUpTime > 2000) {
    needsToBeReset = false;
    targetShape = random(3, 6);
    drawTargetShape();
  }

  if (doClearMistake && millis() - lastMistakeTime > 2500) {
    doClearMistake = false;
    clearMistake();
  }
}

void drawTargetShape() {
  switch (targetShape) {
    case 3: drawTriangle(); break;
    case 4: drawSquare(); break;
    case 5: drawStar(); break;
  }
}

CRGB intToColor(int shape) {
  switch (shape) {
    case 3: return CRGB::Green;
    case 4: return CRGB::Blue;
    case 5: return CRGB::Yellow;
  }
  return CRGB::White;
}
