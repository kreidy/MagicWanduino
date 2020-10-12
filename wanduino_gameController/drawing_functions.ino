/* 
+----+----+----+----+----+----+----+----+
| 7  | 15 | 23 | 31 | 39 | 47 | 55 | 63 |
+----+----+----+----+----+----+----+----+
| 6  | 14 | 22 | 30 | 38 | 46 | 54 | 62 |
+----+----+----+----+----+----+----+----+
| 5  | 13 | 21 | 29 | 37 | 45 | 53 | 61 |
+----+----+----+----+----+----+----+----+
| 4  | 12 | 20 | 28 | 36 | 44 | 52 | 60 |
+----+----+----+----+----+----+----+----+
| 3  | 11 | 19 | 27 | 35 | 43 | 51 | 59 |
+----+----+----+----+----+----+----+----+
| 2  | 10 | 18 | 26 | 34 | 42 | 50 | 58 |
+----+----+----+----+----+----+----+----+
| 1  | 9  | 17 | 25 | 33 | 41 | 49 | 57 |
+----+----+----+----+----+----+----+----+
| 0  | 8  | 16 | 24 | 32 | 40 | 48 | 56 |
+----+----+----+----+----+----+----+----+
*/

void erase() {
  for (int i = 0; i < 64; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
}

void clearArray() {
  for (int i = 0; i < 64; i++) {
    leds[i] = CRGB::Black;
  }
}

void drawSquare() {
  clearArray();
  int led_nums[] = { 18, 19, 20, 21, 26, 29, 34, 37, 42, 43, 44, 45 };
  for (int i = 0; i < 12; i++) {
    leds[led_nums[i]] = CRGB::Blue;
  }
  FastLED.show();
}

void drawTriangle() {
  clearArray();
  int led_nums[] = { 18, 19, 20, 21, 26, 28, 34, 35, 42 };
  for (int i = 0; i < 9; i++) {
    leds[led_nums[i]] = CRGB::Green;
  }
  FastLED.show();
}

void drawStar() {
  clearArray();
  int led_nums[] = { 17, 12, 30, 44, 33, 20, 28, 36, 26, 35, 19, 29 };
  for (int i = 0; i < 12; i++) {
    leds[led_nums[i]] = CRGB::Yellow;
  }
  FastLED.show();
}

void drawMistake() {
  int led_nums[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 16, 24, 15, 23, 31 };
  for (int i = 0; i < 14; i++) {
    leds[led_nums[i]] = CRGB::Red;
    leds[63 - led_nums[i]] = CRGB::Red;
  }
  FastLED.show();
  doClearMistake = true;
  lastMistakeTime = millis();
}

void clearMistake() {
  int led_nums[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 16, 24, 15, 23, 31 };
  for (int i = 0; i < 14; i++) {
    leds[led_nums[i]] = CRGB::Black;
    leds[63 - led_nums[i]] = CRGB::Black;
  }
  FastLED.show();
}

void blowUp(CRGB color) {
  // Frame 1
  int frame1[] = { 27, 28, 35, 36 };
  for (int i = 0; i < 4; i++) {
    leds[frame1[i]] = color;
  }
  FastLED.show();
  delay(100);

  // Frame 2
  for (int i = 0; i < 4; i++) {
    leds[frame1[i]] = CRGB::Black;
  }
  int frame2[] = { 19, 20, 26, 29, 34, 37, 43, 44 };
  for (int i = 0; i < 8; i++) {
    leds[frame2[i]] = color;
  }
  FastLED.show();
  delay(100);

  // Frame 3
  for (int i = 0; i < 8; i++) {
    leds[frame2[i]] = CRGB::Black;
  }
  leds[18] = CRGB::Black;
  leds[21] = CRGB::Black;
  leds[42] = CRGB::Black;
  leds[45] = CRGB::Black;
  int frame3[] = { 10, 11, 12, 13, 17, 22, 25, 30 };
  for (int i = 0; i < 8; i++) {
    leds[frame3[i]] = color;
    leds[63 - frame3[i]] = color;
  }
  FastLED.show();
  delay(100);

  // Frame 4
  for (int i = 0; i < 8; i++) {
    leds[frame3[i]] = CRGB::Black;
    leds[63 - frame3[i]] = CRGB::Black;
  }
  int frame4[] = { 2, 3, 4, 5, 9, 14, 16, 23, 24, 31 };
  for (int i = 0; i < 10; i++) {
    leds[frame4[i]] = color;
    leds[63 - frame4[i]] = color;
  }
  FastLED.show();
  delay(100);

  // Frame 5
  for (int i = 0; i < 10; i++) {
    leds[frame4[i]] = CRGB::Black;
    leds[63 - frame4[i]] = CRGB::Black;
  }
  int frame5[] = { 1, 6, 8, 15, 48, 55, 57, 62 };
  for (int i = 0; i < 8; i++) {
    leds[frame5[i]] = color;
  }
  FastLED.show();
  delay(100);

  // Frame 6
  for (int i = 0; i < 8; i++) {
    leds[frame5[i]] = CRGB::Black;
  }
  int frame6[] = { 0, 7, 56, 63 };
  for (int i = 0; i < 4; i++) {
    leds[frame6[i]] = color;
  }
  FastLED.show();
  delay(100);

  erase();
}
