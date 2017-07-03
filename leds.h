#pragma once

#include "crgb16.h"

namespace leds {
  static const int NUM_LEDS = 30;

  class Animator {
  public:
    virtual void render() = 0;
  };

  CRGB16 getColor();
  void setColor(const CRGB16& color);
  void setColors(const CRGB16* leds);
  void setAnimator(Animator& animator);

  void setup();
  void loop();
  void start(int duration);
  void stop();
}
