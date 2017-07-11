#include "dawn_alarm.h"
#include "fade.h"
#include "leds.h"

#include <FastLED.h>

namespace fade {

  int segmentIndex = 0;

  uint32_t Fader::posToMillis(int segIndex) {
    // if (segIndex >= num_segs) { segIndex = num_segs-1; }
    // if (segIndex < 0) {segIndex = 0; }
    return uint64_t(segments[segIndex].pos)*duration >> 16;
  }

  void Fader::render() {
    uint32_t t = millis() - fadeStartMillis;
#if defined(DEBUG_FADE) || defined(DEBUG_ALARMER)
    t *= 60;
#endif
    DEBUG_FADE_PRINT("t %u", t);
    if (t >= posToMillis(num_segs-1)) {}
    uint32_t end = uint64_t(segments[segmentIndex].pos)*duration >> 16;
    while (segmentIndex < num_segs && end <= t) {
      segmentIndex++;
      end = uint64_t(segments[segmentIndex].pos)*duration >> 16;
    };
    DEBUG_FADE_PRINT(", [%d]", segmentIndex);
    CRGB color  = (segmentIndex == 0 ? CRGB(0,0,0) : segments[segmentIndex-1].color);
    if (segmentIndex >= num_segs) {
      DEBUG_FADE_PRINT(" %02x,%02x,%02x\n", color.r, color.g, color.b);
      leds::setColor(color);
      return;
    }
    uint32_t start = (segmentIndex == 0 ? 0 : uint64_t(segments[segmentIndex -1].pos)*duration >> 16);
    DEBUG_FADE_PRINT(", range [%d..%d]", start, end);
    fract8 f = (uint64_t(t-start) << 8)/(end-start);
    DEBUG_FADE_PRINT(", nblend([%02x,%02x,%02x..%02x,%02x,%02x], .%03d [%d])",
                      color.r, color.g, color.b,
                      segments[segmentIndex].color.r, segments[segmentIndex].color.g, segments[segmentIndex].color.b,
                     (f*1000 >> 8), f);
    CRGB c = nblend(color, CRGB(segments[segmentIndex].color), f);
    DEBUG_FADE_PRINT(", %02x,%02x,%02x\n", c.r, c.g, c.b);
    leds::setColor(c);
    return;
  }
};
