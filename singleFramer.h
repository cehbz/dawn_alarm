#pragma once

#include "leds.h"

  class SingleFramer : public leds::Animator {
  private:
    const CRGB* leds;
  public:
    SingleFramer(const CRGB* l) : leds{l} {};
    void render();
  };
