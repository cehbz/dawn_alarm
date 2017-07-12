#include "dawn_alarm.h"
#include "alarmer.h" // to get DEBUG_ALARMER
#include "fade.h"
#include "leds.h"

#include <FastLED.h>

namespace fade {

  int segmentIndex = 0;

  void Fader::render() {
    uint32_t t = millis() - fadeStartMillis;
#if defined(DEBUG_FADE) || defined(DEBUG_ALARMER)
    t *= 60;
#endif
    DEBUG_FADE_PRINT("t %u", t);
    if (t <= posToMillis(0)) {
      segmentIndex = 0;
    }
    uint32_t end = posToMillis(segmentIndex);
    while (segmentIndex < num_segs && end <= t) {
      segmentIndex++;
      end = posToMillis(segmentIndex);
    };
    DEBUG_FADE_PRINT(", [%d]", segmentIndex);
    CRGB color  = (segmentIndex == 0 ? CRGB(0,0,0) : segments[segmentIndex-1].color);
    if (segmentIndex >= num_segs) {
      DEBUG_FADE_PRINT(" %02x,%02x,%02x\n", color.r, color.g, color.b);
      leds::setColor(color);
      return;
    }
    uint32_t start = (segmentIndex == 0 ? 0 : posToMillis(segmentIndex - 1));
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
