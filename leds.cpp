#include "leds.h"
#include <FastLED.h>

#ifdef DEBUG
#define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#define DEBUG_PRINTLN(...)
#endif

namespace leds {
static const int NUM_LEDS = 30;
static const int DATA_PIN = 1; // D1, GPIO5
CRGB leds[NUM_LEDS];

class CHSVAtTime {
public:
  uint32_t time;
  CHSV color;

  CHSVAtTime(uint32_t t, CHSV c) : time{t}, color{c} {}
};

static const CHSVAtTime segments[] = {
  CHSVAtTime(128, CHSV(HUE_PURPLE, 64, 16)),
  CHSVAtTime(64, CHSV(HUE_BLUE, 96, 32)),
  CHSVAtTime(32, CHSV(HUE_BLUE, 128, 64)),
  CHSVAtTime(16, CHSV(HUE_PINK, 192, 96)),
  CHSVAtTime(8, CHSV(HUE_RED, 255, 128)),
  CHSVAtTime(4, CHSV(HUE_ORANGE, 192, 160)),
  CHSVAtTime(2, CHSV(HUE_YELLOW, 128, 192)),
  CHSVAtTime(1, CHSV(HUE_YELLOW, 64, 255)),
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

void print(CRGB c) {
  DEBUG_PRINT("RGB(0x"); hexPrint(c.r); hexPrint(c.g); hexPrint(c.b); DEBUG_PRINT(")");
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
  for (int i = 0; i < segments_len; i++) {
    sum += segments[i].time;
  }
  DEBUG_PRINT("sum "); DEBUG_PRINTLN(sum);

  FastLED.setMaxPowerInVoltsAndMilliamps(5, 4000);
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  clear();
}

fract8 interpolate8(int x, int a, int b) { // where is x in the range [a..b]?
  return (x-a)*256/(b-a);
}

void print(fract8 f) {
  DEBUG_PRINT(f/256.0);
}

void loop() {
  if (startTime == 0) return;
  uint32_t t = millis()-startTime;
  DEBUG_PRINT("t "); DEBUG_PRINT(t);
  while (segmentIndex < segments_len && endDuration[segmentIndex] < t) {
    segmentIndex++;
  };
  DEBUG_PRINT(", segmentIndex "); DEBUG_PRINT(segmentIndex);
  CHSV color;
  if (segmentIndex < segments_len) {
    uint32_t start = (segmentIndex == 0 ? 0 : endDuration[segmentIndex -1]);
    DEBUG_PRINT(", range ["); DEBUG_PRINT(start); DEBUG_PRINT(".."); DEBUG_PRINT(endDuration[segmentIndex]); DEBUG_PRINT("]");
    DEBUG_PRINT(", pct "); print(interpolate8(t, start, endDuration[segmentIndex]));
    CHSV startColor  = (segmentIndex == 0 ? CHSV(0,0,0) : segments[segmentIndex-1].color);
    DEBUG_PRINT(", startColor "); print(startColor);
    DEBUG_PRINT(", endColor "); print(segments[segmentIndex].color);
    color = blend(startColor, segments[segmentIndex].color, interpolate8(t, start, endDuration[segmentIndex]));
  } else {
    color = segments[segments_len-1].color;
    stop(); // all done
  }
  DEBUG_PRINT(", color "); print(color); DEBUG_PRINT(" "); print(CRGB(color));
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
