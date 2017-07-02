#include "dawn_alarm.h"
#include "fade.h"
#include "leds.h"

namespace fade {

  int segmentIndex = 0;
  uint32_t sum = 0;

  uint16_t blend16(uint16_t existing, uint16_t overlay, fract16 amountOfKeep, fract16 amountOfOverlay )
  {
    return (existing*amountOfKeep + overlay*amountOfOverlay + 0x8000) >> 16;
  }

  CRGB16& nblend16( CRGB16& existing, const CRGB16& overlay, fract16 amountOfOverlay )
  {
    if( amountOfOverlay == 0) {
      return existing;
    }

    if( amountOfOverlay == 0xFFFF) {
      existing = overlay;
      return existing;
    }

    uint16_t amountOfKeep = -amountOfOverlay;

    existing.red = blend16(existing.red, overlay.red, amountOfKeep, amountOfOverlay);
    existing.green = blend16(existing.green, overlay.green, amountOfKeep, amountOfOverlay);
    existing.blue = blend16(existing.blue,  overlay.blue, amountOfKeep, amountOfOverlay);

    return existing;
  }

  void Fader::render(CRGB16* buf) {
    uint32_t t = millis() - fadeStartMillis;
#ifdef DEBUG_FADE
    t *= 60;
#endif
    DEBUG_FADE_PRINT("t %u", t);
    uint32_t end = uint64_t(segments[segmentIndex].pos)*duration/65536;
    while (segmentIndex < num_segs && end <= t) {
      segmentIndex++;
      end = uint64_t(segments[segmentIndex].pos)*duration/65536;
    };
    DEBUG_FADE_PRINT(", [%d]", segmentIndex);
    CRGB16 color  = (segmentIndex == 0 ? CRGB16(0,0,0) : segments[segmentIndex-1].color);
    if (segmentIndex >= num_segs) {
      DEBUG_FADE_PRINT(" %04x,%04x,%04x\n", color.r, color.g, color.b);
      leds::setColor(color);
      return;
    }
    uint32_t start = (segmentIndex == 0 ? 0 : uint64_t(segments[segmentIndex -1].pos)*duration/65536);
    DEBUG_FADE_PRINT(", range [%d..%d]", start, end);
    fract16 f = uint64_t(t-start)*0x10000/(end-start);
    DEBUG_FADE_PRINT(", nblend16([%04x,%04x,%04x..%04x,%04x,%04x], .%03d [%d])",
                      color.r, color.g, color.b,
                      segments[segmentIndex].color.r, segments[segmentIndex].color.g, segments[segmentIndex].color.b,
                      f*1000/0x10000, f);
    CRGB16 c = nblend16(color, CRGB16(segments[segmentIndex].color), f);
    DEBUG_FADE_PRINT(", %04x,%04x,%04x\n", c.r, c.g, c.b);
    leds::setColor(c);
    return;
  }
};
