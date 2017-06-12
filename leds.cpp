#include "leds.h"
#include <FastLED.h>

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
  Serial.print("HSV("); Serial.print(c.h); Serial.print(","); Serial.print(c.s); Serial.print(","); Serial.print(c.v); Serial.print(")");
}

  void hexPrint(uint8_t i) {
    Serial.print(i>>4, HEX); Serial.print(i & 0xf, HEX);
  }

  void hexPrint(uint16_t i) {
    hexPrint(uint8_t(i>>8)); hexPrint(uint8_t(i & 0xff));
  }

void print(CRGB16 c) {
  Serial.print("RGB(0x"); hexPrint(c.r); hexPrint(c.g); hexPrint(c.b); Serial.print(")");
}

void print(CRGB c) {
  Serial.print("RGB(0x"); hexPrint(c.r); hexPrint(c.g); hexPrint(c.b); Serial.print(")");
}

void setEndDurations(uint32_t duration) {
  // compute the start time of each segment
  // starting at the last one
  Serial.print("setEndDurations: "); Serial.println(duration);
  uint32_t end = 0;
  for (int i = 0; i < segments_len; i++) {
    Serial.print("i "); Serial.println(i);
    end += segments[i].time;
    Serial.print("end "); Serial.println(end);
    endDuration[i] = duration*end/sum;
    Serial.println(endDuration[i]);
  }
}

void setup() {
  for (int i = 0; i < segments_len; i++) {
    sum += segments[i].time;
  }
  Serial.print("sum "); Serial.println(sum);

  FastLED.setMaxPowerInVoltsAndMilliamps(5, 4000);
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  clear();
}

fract8 interpolate8(int x, int a, int b) { // where is x in the range [a..b]?
  return (x-a)*256/(b-a);
}

void print(fract8 f) {
  Serial.print(f/256.0);
}

void loop() {
  if (startTime == 0) return;
  uint32_t t = millis()-startTime;
  Serial.print("t "); Serial.print(t);
  while (segmentIndex < segments_len && endDuration[segmentIndex] < t) {
    segmentIndex++;
  };
  Serial.print(", segmentIndex "); Serial.print(segmentIndex);
  CHSV color;
  if (segmentIndex < segments_len) {
    uint32_t start = (segmentIndex == 0 ? 0 : endDuration[segmentIndex -1]);
    Serial.print(", range ["); Serial.print(start); Serial.print(".."); Serial.print(endDuration[segmentIndex]); Serial.print("]");
    Serial.print(", pct "); print(interpolate8(t, start, endDuration[segmentIndex]));
    CHSV startColor  = (segmentIndex == 0 ? CHSV(0,0,0) : segments[segmentIndex-1].color);
    Serial.print(", startColor "); print(startColor);
    Serial.print(", endColor "); print(segments[segmentIndex].color);
    color = blend(startColor, segments[segmentIndex].color, interpolate8(t, start, endDuration[segmentIndex]));
  } else {
    color = segments[segments_len-1].color;
    stop(); // all done
  }
  Serial.print(", color "); print(color); Serial.print(" "); print(CRGB(color));
  fill_solid (leds, NUM_LEDS, color);
  FastLED.show();
  FastLED.delay(50);
  Serial.println();
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
