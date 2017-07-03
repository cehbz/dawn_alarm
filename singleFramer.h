#pragma once

#include "leds.h"

  class SingleFramer : public leds::Animator {
  private:
    const CRGB16* leds;
  public:
    SingleFramer(const CRGB16* l) : leds{l} {};
    void render();
  };
