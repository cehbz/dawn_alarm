#include "dawn_alarm.h"
#include "alarmer.h" // to get DEBUG_ALARMER
#include "fade.h"
#include "leds.h"
#include "singleColor.h"

namespace fade {

  void Fader::endFade(CRGB& endColor) {
    DEBUG_FADE_PRINT(" #%02x%02x%02x\n", endColor.R, endColor.G, endColor.B);
#ifdef DEBUG_ALARMER
    reset();
#else
    static auto a =  singleColor::Monochromer(endColor);
    hwLeds::setAnimator(a);
#endif
  }

  void Fader::render() {
    uint32_t t = millis() - fadeStartMillis;
#if defined(DEBUG_FADE) || defined(DEBUG_ALARMER)
    t *= 60;
#endif
    DEBUG_FADE_PRINT("t %u ", t);
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
    float f = float(t-segStartMillis)/(segEndMillis-segStartMillis);
    DEBUG_FADE_PRINT(", nblend([#%02x%02x%02x..#%02x%02x%02x], .%03d)",
                     segStartColor.R, segStartColor.G, segStartColor.B,
                     segEndColor.R, segEndColor.G, segEndColor.B,
                     int(f*1000));
    CRGB c = RgbwColor::LinearBlend(segStartColor, segEndColor, f);
    DEBUG_FADE_PRINT(", #%02x%02x%02x\n", c.R, c.G, c.B);
    hwLeds::setColor(c);
  }
};
