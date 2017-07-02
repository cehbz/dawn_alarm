#include "dawn_alarm.h"
#include "ntp.h"
#include "server.h"

#include <TimeAlarms.h>

#include "fade.h"

// turn num/den into a fract16
static const fract16 f16(const uint16_t num, const uint16_t den) {
  return min(65535, (uint32_t(num)*65536+den/2)/den);
}

// CRGB16(0x1000, 0x0400, 0x0080)
static const fade::ColorAtTime segments[] = {
  fade::ColorAtTime(f16(8,48), CRGB(1, 2, 4)),
  fade::ColorAtTime(f16(24,48), CRGB(4, 4, 13)),
  fade::ColorAtTime(f16(32,48), CRGB(21, 2, 11)),
  fade::ColorAtTime(f16(40,48), CRGB(40, 0, 5)),
  fade::ColorAtTime(f16(44,48), CRGB(128, 73, 20)),
  fade::ColorAtTime(f16(48,48), CRGB(Candle)),
};
static const int num_segs = sizeof(segments)/sizeof(segments[0]);
static fade::Fader fader(segments, num_segs, 1*SECS_PER_HOUR*1000, -1);

void MorningAlarm() {
  Serial.println("MorningAlarm");
  fader = fade::Fader(segments, num_segs, 1*SECS_PER_HOUR*1000, millis());
#ifdef DEBUG_FADE
  for (int i = 0; i < num_segs; i++) {
    auto s = segments[i];
    DEBUG_FADE_PRINT("segments[%d] {%04x [.%3d], {%d, %d, %d}}\n",
                     i,
                     s.pos,
                     s.pos*1000/65536,
                     s.color.r,
                     s.color.g,
                     s.color.b);
  }
  DEBUG_FADE_PRINT("duration %d\n", 1*SECS_PER_HOUR*1000);
#endif
  leds::setAnimator(fader);
}

void setup() {
  Serial.begin(115200);
  delay(250);
  Serial.println("\nbooted\n");
  ntp::setup();
  leds::setup();
  server::setup();
#ifndef DEBUG_FADE
  Alarm.alarmRepeat(6,0,0, MorningAlarm);
#else
  Alarm.alarmOnce(hour(now()), minute(now()), second(now())+1, MorningAlarm);
#endif
}

void loop() {
  ntp::loop();
  leds::loop();
  server::loop();
  Alarm.delay(0);
}
