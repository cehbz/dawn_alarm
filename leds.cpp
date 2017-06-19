#include "dawn_alarm.h"
#include "leds.h"
#include <FastLED.h>
#include "crgb16.h"

namespace leds {
  static const int NUM_LEDS = 30;
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
    ColorAtTime(4, CRGB(128, 73, 20)),
    ColorAtTime(4, CRGB(Candle)),
  };

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

  // #define LEDS_DEBUG
#undef LEDS_DEBUG

#ifdef LEDS_DEBUG
#define LEDS_DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#define LEDS_DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
#define LEDS_DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#else
#define LEDS_DEBUG_PRINTF(...) 0
#define LEDS_DEBUG_PRINT(...) 0
#define LEDS_DEBUG_PRINTLN(...) 0
#endif

  CRGB16 getColor() {
    uint32_t t = millis()-startTime;
    LEDS_DEBUG_PRINTF("t %d", t);
    while (segmentIndex < segments_len && endDuration[segmentIndex] <= t) {
      segmentIndex++;
    };
    LEDS_DEBUG_PRINTF(", [%d]", segmentIndex);
    if (segmentIndex >= segments_len) {
      stop(); // all done
      return segments[segments_len-1].color;
    }
    uint32_t start = (segmentIndex == 0 ? 0 : endDuration[segmentIndex -1]);
    LEDS_DEBUG_PRINTF(", range [%d..%d]", start, endDuration[segmentIndex]);
    CRGB16 color  = (segmentIndex == 0 ? CRGB16(0,0,0) : segments[segmentIndex-1].color);
    fract16 f = (t-start)*(0x10000)/(endDuration[segmentIndex]-start);
    LEDS_DEBUG_PRINTF(", nblend16([%04x%04x%04x..%04x%04x%04x], .%03d [%d])",
                      color.r, color.g, color.b,
                      segments[segmentIndex].color.r, segments[segmentIndex].color.g, segments[segmentIndex].color.b,
                      f*1000/(0x10000), f);
    return nblend16(color, CRGB16(segments[segmentIndex].color), f);
  }

  void updateComponentAndError(uint16_t& component, int16_t& error) {
    int c = component + error;
    if (c < 0) {
      error = c;
      component = 0;
      LEDS_DEBUG_PRINT("<");
    } else if (c > 65535) {
      error = c - 65535;
      component = 65535;
      LEDS_DEBUG_PRINT(">");
    } else {
      error = 0;
      component = c;
      LEDS_DEBUG_PRINT("=");
    }
  }

  int16_t errorR = 0;
  int16_t errorG = 0;
  int16_t errorB = 0;
  void setLeds(CRGB16& color) {
    for (int i = 0; i<NUM_LEDS; i++) {
      CRGB16 c(color);
      LEDS_DEBUG_PRINTF("start error: %+d, %+d, %+d ", errorR, errorG, errorB);
      updateComponentAndError(c.r, errorR);
      updateComponentAndError(c.g, errorG);
      updateComponentAndError(c.b, errorB);
      CRGB c8 = c.CRGB16to8();
      leds[i] = c8;
      LEDS_DEBUG_PRINTF(", led[%d] %02x%02x%02x", i, leds[i].r, leds[i].g, leds[i].b);
      errorR += c.r-(c8.r<<8);
      errorG += c.g-(c8.g<<8);
      errorB += c.b-(c8.b<<8);
      LEDS_DEBUG_PRINTF(", end error: %+d, %+d, %+d", errorR, errorG, errorB);
      LEDS_DEBUG_PRINTLN();
    }
  }

  void loop() {
    if (startTime == 0) return; // not running
    CRGB16 color = getColor();
    LEDS_DEBUG_PRINTF(", color %04x%04x%04x\n", color.r, color.g, color.b);
    setLeds(color);
    FastLED.show();
    FastLED.delay(0);
    LEDS_DEBUG_PRINTLN();
    frames++;
    if (millis()>=fpsEndTime) {
      DEBUG_PRINTF("%d fps\n", frames);
      fpsEndTime = millis()+1000;
      frames = 0;
    }
  }

  void start(int duration) {
    setEndDurations(duration);
    segmentIndex = 0;
    startTime = millis();
  }

  void stop() {
    startTime = 0;
  }

  void clear() {
    stop();
    fill_solid (leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
  }
}
