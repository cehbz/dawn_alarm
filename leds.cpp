#include "dawn_alarm.h"
#include "leds.h"
#include <FastLED.h>

namespace leds {
  static const int NUM_LEDS = 30;
  static const int DATA_PIN = 1; // D1, GPIO5
  CRGB leds[NUM_LEDS];

  /// Representation of a 16 bit RGB pixel (Red, Green, Blue)
  struct CRGB16 {
    union {
      struct {
        union {
          uint16_t r;
          uint16_t red;
        };
        union {
          uint16_t g;
          uint16_t green;
        };
        union {
          uint16_t b;
          uint16_t blue;
        };
      };
      uint16_t raw[3];
    };

    // default values are UNINITIALIZED
    inline CRGB16() __attribute__((always_inline))
    {
    }

    // allow construction from 16 bit R, G, B
    inline CRGB16( uint16_t ir, uint16_t ig, uint16_t ib)  __attribute__((always_inline))
      : r(ir), g(ig), b(ib)
    {
    }

    // // allow construction from 8 bit R, G, B
    // inline CRGB16( uint8_t ir, uint8_t ig, uint8_t ib)  __attribute__((always_inline))
    //   : r(ir<<8), g(ig<<8), b(ib<<8)
    // {
    // }

    // allow copy construction
    inline CRGB16(const CRGB16& rhs) __attribute__((always_inline))
      : r(rhs.r), g(rhs.g), b(rhs.b)
    {
    }

    // allow construction from 8 bit CRGB
    inline CRGB16(const CRGB& rhs) __attribute__((always_inline))
      : r(rhs.r<<8), g(rhs.g<<8), b(rhs.b<<8)
    {
    }

    // allow assignment from 8 bit CRGB
    inline CRGB16& operator= (const CRGB& rhs) __attribute__((always_inline))
    {
      r = rhs.r << 8;
      g = rhs.g << 8;
      b = rhs.b << 8;
      return *this;
    }
  };

  CRGB CRGB16to8(const CRGB16& c) {return CRGB(c.r>>8, c.g>>8, c.b>>8);}

  class ColorAtTime {
  public:
    uint32_t time;
    CRGB16 color;

    // ColorAtTime(uint32_t t, CHSV c) : time{t}, color{CRGB16(c)} {}
    ColorAtTime(uint32_t t, CRGB16 c) : time{t}, color{c} {}
  };

  // static const ColorAtTime segments[] = {
  //   ColorAtTime(128, CHSV(HUE_PURPLE, 64, 16)),
  //   ColorAtTime(64, CHSV(HUE_BLUE, 96, 32)),
  //   ColorAtTime(32, CHSV(HUE_BLUE, 128, 64)),
  //   ColorAtTime(16, CHSV(HUE_PINK, 192, 96)),
  //   ColorAtTime(8, CHSV(HUE_RED, 255, 128)),
  //   ColorAtTime(4, CHSV(HUE_ORANGE, 192, 160)),
  //   ColorAtTime(2, CHSV(HUE_YELLOW, 128, 192)),
  //   ColorAtTime(1, CHSV(HUE_YELLOW, 64, 255)),
  // };

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
    // compute the start time of each segment
    // starting at the last one
    DEBUG_PRINT("setEndDurations: "); DEBUG_PRINTLN(duration);
    uint32_t end = 0;
    for (int i = 0; i < segments_len; i++) {
      DEBUG_PRINTF("segment[%d] [%d..%d) ", i, end, end + segments[i].time);
      end += segments[i].time;
      endDuration[i] = duration*end/sum;
      DEBUG_PRINTLN(endDuration[i]);
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

  void setup() {
    for (int i = 0; i < segments_len; i++) {
      sum += segments[i].time;
    }
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 4000);
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
    clear();
  }

  int16_t errorR = 0;
  int16_t errorG = 0;
  int16_t errorB = 0;
  uint32_t fpsEndTime = millis()+1000;
  uint32_t frames = 0;
  void loop() {
    if (startTime == 0) return;
    uint32_t t = millis()-startTime;
    DEBUG_PRINTF("t %d", t);
    while (segmentIndex < segments_len && endDuration[segmentIndex] <= t) {
      segmentIndex++;
    };
    DEBUG_PRINTF(", [%d]", segmentIndex);
    CRGB16 color;
    if (segmentIndex < segments_len) {
      uint32_t start = (segmentIndex == 0 ? 0 : endDuration[segmentIndex -1]);
      DEBUG_PRINTF(", range [%d..%d]", start, endDuration[segmentIndex]);
      color  = (segmentIndex == 0 ? CRGB16(0,0,0) : segments[segmentIndex-1].color);
      fract16 f = (t-start)*(0x10000)/(endDuration[segmentIndex]-start);
      DEBUG_PRINTF(", nblend16([%04x%04x%04x..%04x%04x%04x], .%03d [%d])",
                   color.r, color.g, color.b,
                   segments[segmentIndex].color.r, segments[segmentIndex].color.g, segments[segmentIndex].color.b,
                   f*1000/(0x10000), f);
      color = nblend16(color, CRGB16(segments[segmentIndex].color), f);
    } else {
      color = segments[segments_len-1].color;
      stop(); // all done
    }
    DEBUG_PRINTF(", color %04x%04x%04x\n", color.r, color.g, color.b);
    for (int i = 0; i<NUM_LEDS; i++) {
      DEBUG_PRINTF("start error: %d, %d, %d ", errorR, errorG, errorB);
      CRGB16 c(color.r+errorR, color.g+errorG, color.b+errorB);
      CRGB c8 = CRGB16to8(c);
      leds[i] = c8;
      DEBUG_PRINTF(", led[%d] %02x%02x%02x", i, leds[i].r, leds[i].g, leds[i].b);
      errorR = c.r-(c8.r<<8);
      errorG = c.g-(c8.g<<8);
      errorB = c.b-(c8.b<<8);
      DEBUG_PRINTF(", end error: %d, %d, %d", errorR, errorG, errorB);
      DEBUG_PRINTLN();
    }
    FastLED.show();
    FastLED.delay(0);
    DEBUG_PRINTLN();
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
