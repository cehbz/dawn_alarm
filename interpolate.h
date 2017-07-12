#pragma once

#include "leds.h"

namespace interpolate {
  class Interpolater : public leds::Animator {
  private:
    CRGB start;
    CRGB end;
    bool useStart;
  public:
    Interpolater(const CRGB& s, const CRGB& e) : start{s}, end{e}, useStart{true} {};
    void render();
    void print() {
      Serial.printf("Interpolater(#%02x%02x%02x, %02x%02x%02x)", start.r, start.g, start.b, end.r, end.g, end.b);
    }
  };
};
