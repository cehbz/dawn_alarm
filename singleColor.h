#pragma once

#include "leds.h"

namespace singleColor {
  class Monochromer : public Animator {
  private:
    // const prevents copy construction and assignment.
    CRGB color;
  public:
    Monochromer(const CRGB& c) : color{c} {};
    void render();
    void print() { Serial.printf("Monochromer(%02x%02x%02x)", color.r, color.g, color.b); }
  };
};
