#pragma once

#include "leds.h"

namespace singleColor {
  class Monochromer : public leds::Animator {
  private:
    // const prevents copy construction and assignment.
    CRGB color;
  public:
    Monochromer(const CRGB& c) : color{c} {};
    void render();
  };
};
