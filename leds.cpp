#include "dawn_alarm.h"
#include "leds.h"
#include <FastLED.h>
#include "crgb16.h"
#include "crgb32.h"

namespace leds {
  static const int DATA_PIN = 1; // D1, GPIO5
  CRGB leds[NUM_LEDS];

  class ColorAtTime {
  public:
    uint32_t time;
    CRGB16 color;

    ColorAtTime(uint32_t t, CRGB16 c) : time{t}, color{c} {}
  };

static const ColorAtTime segments[] = {
    ColorAtTime(8, CRGB(1,2,4)),
    ColorAtTime(16, CRGB(4, 4, 13)),
    ColorAtTime(8, CRGB(21, 2, 11)),
    ColorAtTime(8, CRGB(40, 0, 5)),
    // ColorAtTime(8, CRGB16(0x1000, 0x0400, 0x0080)),
    ColorAtTime(4, CRGB(128, 73, 20)),
    ColorAtTime(4, CRGB(Candle)),
  };

#define DEBUG_LEDS
  static const int segments_len = sizeof(segments)/sizeof(segments[0]);
  uint32_t endDuration[segments_len];
  int segmentIndex = 0;
  uint32_t sum = 0;
  uint32_t startTime = 0;

  void setEndDurations(uint32_t duration) {
    uint32_t end = 0;
    for (int i = 0; i < segments_len; i++) {
      end += segments[i].time;
      endDuration[i] = duration*end/sum;
    }
  }

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

  uint32_t fpsEndTime;
  uint32_t frames;
  void setup() {
    for (int i = 0; i < segments_len; i++) {
      sum += segments[i].time;
    }
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 4000);
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
    clear();
    fpsEndTime = millis()+1000;
    frames = 0;
  }

  CRGB16 lastSetColor;
  CRGB16 getColor() {
    if (startTime == 0) {
      return lastSetColor;
    }
#ifndef DEBUG_DAWN
    uint32_t t = millis();
#else
    uint32_t t = millis()*60;
#endif
    if (t <= startTime) {
      return CRGB16(0,0,0);
    }
    t -= startTime;
    DEBUG_LEDS_PRINT("t %u", t);
    while (segmentIndex < segments_len && endDuration[segmentIndex] <= t) {
      segmentIndex++;
    };
    DEBUG_LEDS_PRINT(", [%d]", segmentIndex);
    if (segmentIndex >= segments_len) {
      stop(); // all done
      return segments[segments_len-1].color;
    }
    uint32_t start = (segmentIndex == 0 ? 0 : endDuration[segmentIndex -1]);
    DEBUG_LEDS_PRINT(", range [%d..%d]", start, endDuration[segmentIndex]);
    CRGB16 color  = (segmentIndex == 0 ? CRGB16(0,0,0) : segments[segmentIndex-1].color);
    fract16 f = uint64_t(t-start)*0x10000/(endDuration[segmentIndex]-start);
    DEBUG_LEDS_PRINT(", nblend16([%04x%04x%04x..%04x%04x%04x], .%03d [%d])",
                      color.r, color.g, color.b,
                      segments[segmentIndex].color.r, segments[segmentIndex].color.g, segments[segmentIndex].color.b,
                      f*1000/0x10000, f);
    return nblend16(color, CRGB16(segments[segmentIndex].color), f);
  }

  void updateOneComponentAndError(uint16_t& component, int16_t& error) {
    int c = component + error;
    if (c < 0) {
      error = c;
      component = 0;
      DEBUG_LEDS_PRINT("<");
    } else if (c > 65535) {
      error = c - 65535;
      component = 65535;
      DEBUG_LEDS_PRINT(">");
    } else {
      error = 0;
      component = c;
      DEBUG_LEDS_PRINT("=");
    }
  }

  struct Error16 {
    int16_t r;
    int16_t g;
    int16_t b;

  // default values are UNINITIALIZED
  inline Error16() __attribute__((always_inline)) {}

  // allow construction from signed 16 bit R, G, B
  inline Error16( int16_t ir, int16_t ig, int16_t ib)  __attribute__((always_inline)) : r(ir), g(ig), b(ib) {}
  };

  void updateComponentAndError(CRGB16& c, Error16& e) {
    updateOneComponentAndError(c.r, e.r);
    updateOneComponentAndError(c.g, e.g);
    updateOneComponentAndError(c.b, e.b);
  }

  Error16 error(0,0,0);

  void setColors(const CRGB16* leds16) {
    CRGB32 avg(0,0,0);
    for (int i = 0; i<NUM_LEDS; i++) {
      CRGB16 c(leds16[i]);
      DEBUG_LEDS_PRINT("start error: %+4d, %+4d, %+4d ", error.r, error.g, error.b);
      updateComponentAndError(c, error);
      CRGB c8 = c.CRGB16to8();
      leds[i] = c8;
      DEBUG_LEDS_PRINT(", led[%2d] %02x%02x%02x", i, leds[i].r, leds[i].g, leds[i].b);
      CRGB16 c16(c8);
      avg.r += c16.r;
      avg.g += c16.g;
      avg.b += c16.b;
      error.r += c.r-c16.r;
      error.g += c.g-c16.g;
      error.b += c.b-c16.b;
      DEBUG_LEDS_PRINT(", end error: %+4d, %+4d, %+4d", error.r, error.g, error.b);
      DEBUG_LEDS_PRINT("\n");
    }
    lastSetColor = CRGB16(
                          (avg.r+NUM_LEDS/2)/NUM_LEDS,
                          (avg.g+NUM_LEDS/2)/NUM_LEDS,
                          (avg.b+NUM_LEDS/2)/NUM_LEDS);
    FastLED.show();
    FastLED.delay(0);
  }

  void setColor(const CRGB16& color) {
    CRGB16 leds16[NUM_LEDS];
    for (int i = 0; i<NUM_LEDS; i++) {
      leds16[i] = color;
    }
    setColors(leds16);
  }

  void loop() {
    if (startTime == 0) return; // not running
    CRGB16 color = getColor();
    DEBUG_LEDS_PRINT(", color %04x%04x%04x\n", color.r, color.g, color.b);
    setColor(color);
    DEBUG_LEDS_PRINT("\n");
    frames++;
    if (millis()>=fpsEndTime) {
      DEBUG_PRINT("@%lu, %d fps [%04x, %04x, %04x]\n", millis(), frames, color.r, color.g, color.b);
      fpsEndTime = millis()+1000;
      frames = 0;
    }
  }

  void start(int duration) {
    setEndDurations(duration);
    segmentIndex = 0;
#ifndef DEBUG_DAWN
    startTime = millis();
#else
    startTime = millis()*60;
#endif
    DEBUG_PRINT("start: startTime: %d\n", startTime);
  }

  void stop() {
  startTime = 0;
  DEBUG_PRINT("stop: startTime: %d\n", startTime);
  }

  void clear() {
    stop();
    fill_solid (leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
  }
}
