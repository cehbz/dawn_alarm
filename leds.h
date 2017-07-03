#pragma once

#include <FastLED.h>
#include "crgb16.h"

namespace leds {
  static const int NUM_LEDS = 30;

  class Animator {
  public:
    virtual void render() = 0;
  };

  CRGB getColor();
  void setColor(const CRGB& color);
  void setColors(const CRGB* leds);
  void setAnimator(Animator& animator);

  void setup();
  void loop();
  void start(int duration);
  void stop();
}
