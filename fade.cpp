#include "options.h"
#include "fade.h"
#include "leds.h"
#include "singleColor.h"

void Fader::endFade(CRGB& endColor) {
  if (options::debug_fade) Serial.printf(" #%02x%02x%02x\n", endColor.R, endColor.G, endColor.B);
  if (options::debug_alarmer) {
    reset();
    return;
  }
  static auto a =  Monochromer(endColor);
  leds::setAnimator(a);
}

void Fader::render() {
  uint32_t t = millis() - fadeStartMillis;
  if (options::debug_fade || options::debug_alarmer) {
    t *= 60;
  }
  if (options::debug_fade) Serial.printf("t %u ", t);
  while (t > segEndMillis) {
    segmentIndex++;
    if (segmentIndex >= num_segs) {
      endFade(segEndColor);
      return;
    }
    setIndex(segmentIndex);
  };
  if (options::debug_fade) Serial.printf(", [%d]", segmentIndex);
  if (options::debug_fade) Serial.printf(", range [%d..%d]", segStartMillis, segEndMillis);
  float f = float(t-segStartMillis)/(segEndMillis-segStartMillis);
  if (options::debug_fade) {
    Serial.printf(", nblend([#%02x%02x%02x..#%02x%02x%02x], .%03d)",
                  segStartColor.R, segStartColor.G, segStartColor.B,
                  segEndColor.R, segEndColor.G, segEndColor.B,
                  int(f*1000));
  }
  CRGB c = RgbwColor::LinearBlend(segStartColor, segEndColor, f);
  if (options::debug_fade) Serial.printf(", #%02x%02x%02x\n", c.R, c.G, c.B);
  leds::setColor(c);
}
