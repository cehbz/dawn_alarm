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
    float pos;
    CRGB color;

    ColorAtTime(float f, CRGB c) : pos{f}, color{c} {}
  };

  class Fader : public Animator {
  private:
    const ColorAtTime* segments;
    uint8_t num_segs;
    uint32_t duration;

    uint32_t fadeStartMillis;
    int segmentIndex;
    uint32_t segStartMillis;
    uint32_t segEndMillis;
    CRGB segStartColor;
    CRGB segEndColor;

    void setIndex(int segIndex) {
      segmentIndex = segIndex;
      segStartMillis = (segIndex == 0 ? 0 : posToMillis(segIndex-1));
      segEndMillis = posToMillis(segIndex);
      segStartColor = (segIndex == 0 ? CRGB(0,0,0) : segments[segIndex-1].color);
      segEndColor = segments[segIndex].color;
    }
    void endFade(CRGB& endColor);
  public:
    Fader(const ColorAtTime* segs, const uint8_t num, const uint32_t dur)
      : segments{segs},
        num_segs{num},
        duration{dur}
    {
      reset();
    };

    inline uint32_t posToMillis(int segIndex) __attribute__ ((always_inline)) {
      return segments[segIndex].pos*duration;
    }

    void print() {
      Serial.printf(
                    "Fader(*%08x, %d, %d, @%d, %d, @%d, @%d, #%02x%02x%02x, #%02x%02x%02x)",
                    uint32_t(segments),
                    num_segs,
                    duration,
                    fadeStartMillis,
                    segmentIndex,
                    segStartMillis,
                    segEndMillis,
                    segStartColor.R, segStartColor.G, segStartColor.B,
                    segEndColor.R, segEndColor.G, segEndColor.B);
    };

    void render();
    void reset() {
      fadeStartMillis = millis();
      Serial.printf("@%lu fade.reset() fadeStartMillis = %lu\n", millis(), fadeStartMillis);
      setIndex(0);
    }
  };
};
