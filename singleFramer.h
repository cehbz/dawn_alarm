#pragma once

#include "leds.h"

  class SingleFramer : public Animator {
  private:
    const CRGB* leds;
  public:
    SingleFramer(const CRGB* l) : leds{l} {};
    void render();
    void print() { Serial.printf("SingleFramer(*%x)", uint32_t(leds)); }
  };
