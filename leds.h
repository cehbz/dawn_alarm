#pragma once

#include <FastLED.h>
#include "crgb16.h"

class Animator {
public:
  virtual void render() = 0;
  virtual void print() = 0;
};

class hwLeds {
public:
  static const int NUM_LEDS = 30;
  static const CRGB getColor();
  static const CRGB* getColors();
  static void setColor(const CRGB& color);
  static void setColors(const CRGB* leds);
  static void setAnimator(Animator& animator);

  static void setup();
  static void loop();
};
