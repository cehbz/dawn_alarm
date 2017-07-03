#pragma once

#include "leds.h"

// #define DEBUG_FADE

#ifdef DEBUG_FADE
#define DEBUG_FADE_PRINT(...) Serial.printf(__VA_ARGS__)
#else
#define DEBUG_FADE_PRINT(...)
#endif

namespace fade {
  class ColorAtTime {
  public:
    fract16 pos;
    CRGB color;

    ColorAtTime(fract16 f, CRGB c) : pos{f}, color{c} {}
  };

  class Fader : public leds::Animator {
  private:
    const ColorAtTime* segments;
    uint8_t num_segs;
    uint32_t duration;
    uint32_t fadeStartMillis;
  public:
    Fader(const ColorAtTime* segs, const uint8_t num, const uint32_t dur, const uint32_t start = millis())
      : segments{segs},
        num_segs{num},
        duration{dur},
        fadeStartMillis{start}
    {};
    void render();
  };
};
