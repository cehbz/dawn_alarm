#include "dawn_alarm.h"
#include "leds.h"
#include <FastLED.h>

namespace leds {
static const int NUM_LEDS = 30;
static const int DATA_PIN = 1; // D1, GPIO5
CRGB leds[NUM_LEDS];

class ColorAtTime {
public:
  uint32_t time;
  CRGB color;

  ColorAtTime(uint32_t t, CHSV c) : time{t}, color{CRGB(c)} {}
  ColorAtTime(uint32_t t, CRGB c) : time{t}, color{c} {}
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
  ColorAtTime(4, CRGB(1,2,4)),
  ColorAtTime(8, CRGB(4, 4, 13)),
  ColorAtTime(2, CRGB(21, 2, 11)),
  ColorAtTime(2, CRGB(40, 0, 5)),
  ColorAtTime(1, CRGB(128, 73, 20)),
  // ColorAtTime(8, Candle),
};

static const int segments_len = sizeof(segments)/sizeof(segments[0]);
uint32_t endDuration[segments_len];
int segmentIndex = 0;
uint32_t sum = 0;
uint32_t startTime = 0;

void print(CHSV c) {
  DEBUG_PRINT("HSV("); DEBUG_PRINT(c.h); DEBUG_PRINT(","); DEBUG_PRINT(c.s); DEBUG_PRINT(","); DEBUG_PRINT(c.v); DEBUG_PRINT(")");
}

  void hexPrint(uint8_t i) {
    DEBUG_PRINT(i>>4, HEX); DEBUG_PRINT(i & 0xf, HEX);
  }

  void hexPrint(uint16_t i) {
    hexPrint(uint8_t(i>>8)); hexPrint(uint8_t(i&0xff));
  }

void print(CRGB c) {
  DEBUG_PRINT("RGB(0x"); hexPrint(c.r); hexPrint(c.g); hexPrint(c.b); DEBUG_PRINT(")");
}

void print(fract8 f) {
  DEBUG_PRINT(f/256.0);
  DEBUG_PRINT(" ("); hexPrint(f); DEBUG_PRINT(")");
}
void setEndDurations(uint32_t duration) {
  // compute the start time of each segment
  // starting at the last one
  DEBUG_PRINT("setEndDurations: "); DEBUG_PRINTLN(duration);
  uint32_t end = 0;
  for (int i = 0; i < segments_len; i++) {
    DEBUG_PRINT("i "); DEBUG_PRINTLN(i);
    end += segments[i].time;
    DEBUG_PRINT("end "); DEBUG_PRINTLN(end);
    endDuration[i] = duration*end/sum;
    DEBUG_PRINTLN(endDuration[i]);
  }
}

void setup() {
  DEBUG_PRINT("blend(0x000000,0x010204,0xFE) is "); print(blend(0x000000,0x010204,0xfe)); DEBUG_PRINTLN(" should be 010204");
  DEBUG_PRINT("blend(0x010204,0x020408,0x01) is "); print(blend(0x010204,0x020408,0x01)); DEBUG_PRINTLN(" should be 010204");
  DEBUG_PRINT("blend(0x010204,0x020408,0xFE) is "); print(blend(0x010204,0x020408,0xfe)); DEBUG_PRINTLN(" should be 020408");
  for (int i = 0; i < segments_len; i++) {
    sum += segments[i].time;
  }
  DEBUG_PRINT("sum "); DEBUG_PRINTLN(sum);

  FastLED.setMaxPowerInVoltsAndMilliamps(5, 4000);
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  clear();
}

void loop() {
  if (startTime == 0) return;
  uint32_t t = millis()-startTime;
  DEBUG_PRINT("t "); DEBUG_PRINT(t);
  while (segmentIndex < segments_len && endDuration[segmentIndex] <= t) {
    segmentIndex++;
  };
  DEBUG_PRINT(", ["); DEBUG_PRINT(segmentIndex); DEBUG_PRINT("]");
  CRGB color;
  if (segmentIndex < segments_len) {
    uint32_t start = (segmentIndex == 0 ? 0 : endDuration[segmentIndex -1]);
    DEBUG_PRINT(", range ["); DEBUG_PRINT(start); DEBUG_PRINT(".."); DEBUG_PRINT(endDuration[segmentIndex]); DEBUG_PRINT("]");
    color  = (segmentIndex == 0 ? CRGB(0,0,0) : segments[segmentIndex-1].color);
    DEBUG_PRINT(", nblend([");
    print(color);
    DEBUG_PRINT(".."); print(segments[segmentIndex].color);
    fract8 f = (t-start)*256/(endDuration[segmentIndex]-start);
    DEBUG_PRINT("], "); print(f);
    color = nblend(color, segments[segmentIndex].color, f);
    DEBUG_PRINT(")");
  } else {
    color = segments[segments_len-1].color;
    stop(); // all done
  }
  DEBUG_PRINT(", color "); print(color);
  fill_solid (leds, NUM_LEDS, color);
  FastLED.show();
  FastLED.delay(50);
  DEBUG_PRINTLN();
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
