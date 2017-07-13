#include "dawn_alarm.h"
#include "alarmer.h" // to get DEBUG_ALARMER
#include "fade.h"
#include "leds.h"
#include "singleColor.h"

#include <FastLED.h>

namespace fade {

  void Fader::endFade(CRGB& endColor) {
    DEBUG_FADE_PRINT(" #%02x%02x%02x\n", endColor.r, endColor.g, endColor.b);
#ifdef DEBUG_ALARMER
    reset();
#else
    static auto a =  singleColor::Monochromer(endColor);
    leds::setAnimator(a);
#endif
  }

  void Fader::render() {
    uint32_t t = millis() - fadeStartMillis;
#if defined(DEBUG_FADE) || defined(DEBUG_ALARMER)
    t *= 60;
#endif
    DEBUG_FADE_PRINT("t %u", t);
    while (t > segEndMillis) {
      segmentIndex++;
      if (segmentIndex >= num_segs) {
        endFade(segEndColor);
        return;
      }
      setIndex(segmentIndex);
    };
    DEBUG_FADE_PRINT(", [%d]", segmentIndex);
    DEBUG_FADE_PRINT(", range [%d..%d]", segStartMillis, segEndMillis);
    fract8 f = (uint64_t(t-segStartMillis) << 8)/(segEndMillis-segStartMillis);
    DEBUG_FADE_PRINT(", nblend([#%02x%02x%02x..#%02x%02x%02x], .%03d [%d/256])",
                     segStartColor.r, segStartColor.g, segStartColor.b,
                     segEndColor.r, segEndColor.g, segEndColor.b,
                     (f*1000 >> 8), f);
    CRGB c = segStartColor;
    c = nblend(c, segEndColor, f);
    DEBUG_FADE_PRINT(", #%02x%02x%02x\n", c.r, c.g, c.b);
    leds::setColor(c);
  }
};
