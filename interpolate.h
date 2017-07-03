#pragma once

#include "leds.h"

namespace interpolate {
  class Interpolater : public leds::Animator {
  private:
    CRGB16 start;
    CRGB16 end;
    bool useStart;
  public:
    Interpolater(const CRGB16& s, const CRGB16& e) : start{s}, end{e}, useStart{true} {};
    void render();
  };
};
