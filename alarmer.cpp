#include <TimeAlarms.h>

#include "alarmer.h"
#include "fade.h"

namespace alarmer {
  // turn num/den into a fract16
  static const fract16 f16(const uint16_t num, const uint16_t den) {
    return min(65535, (uint32_t(num)*65536+den/2)/den);
  }

  // CRGB16(0x1000, 0x0400, 0x0080)
  static const fade::ColorAtTime segments[] = {
    fade::ColorAtTime(f16(3,7), CRGB(28, 41, 56)),
    fade::ColorAtTime(f16(5,7), CRGB(56, 56, 87)),
    fade::ColorAtTime(f16(6,7), CRGB(124, 79, 18)),
    fade::ColorAtTime(f16(7,7), CRGB(255, 209, 132)),
    // fade::ColorAtTime(f16(8,48), CRGB(28, 41, 56)),
    // fade::ColorAtTime(f16(24,48), CRGB(56, 56, 87)),
    // fade::ColorAtTime(f16(32,48), CRGB(103, 87, 42)),
    // fade::ColorAtTime(f16(40,48), CRGB(124, 79, 18)),
    // fade::ColorAtTime(f16(44,48), CRGB(199, 160, 102)),
    // fade::ColorAtTime(f16(48,48), CRGB(255, 209, 132)),
  };
  static const int num_segs = sizeof(segments)/sizeof(segments[0]);
  static fade::Fader fader(segments, num_segs, 1*SECS_PER_HOUR*1000);

  void MorningAlarm() {
    Serial.println("MorningAlarm");
    fader.reset();
    Serial.printf("@%lu ", millis()); fader.print(); Serial.println();
#ifdef DEBUG_ALARMER
    for (int i = 0; i < num_segs; i++) {
      auto s = segments[i];
      DEBUG_ALARMER_PRINT("segments[%d] {%04x [.%3d], {%d, %d, %d}}\n",
                       i,
                       s.pos,
                       s.pos*1000/65536,
                       s.color.r,
                       s.color.g,
                       s.color.b);
    }
    DEBUG_ALARMER_PRINT("duration %d\n", 1*SECS_PER_HOUR*1000);
#endif
    leds::setAnimator(fader);
  }

  AlarmID_t dowAlarms[dowSaturday];
  void SetAlarm(const timeDayOfWeek_t dow, const int h, const int m) {
    Alarm.free(dowAlarms[dow]);
    dowAlarms[dow] = Alarm.alarmRepeat(dow, h, m, 0, MorningAlarm);
  }

  void setup() {
    for ( int d = dowSunday; d <= dowSaturday; d++) {
      timeDayOfWeek_t dow = timeDayOfWeek_t(d);
      switch (dow) {
      case dowSaturday:
      case dowSunday:
      case dowMonday:
        dowAlarms[dow] = Alarm.alarmRepeat(dow, 7, 0, 0, MorningAlarm);
        break;
      default:
        dowAlarms[dow] = Alarm.alarmRepeat(dow, 6, 0, 0, MorningAlarm);
      }
    }
#ifdef DEBUG_ALARMER
    Alarm.timerOnce(1, MorningAlarm);
#else
    Alarm.alarmRepeat(6, 0, 0, MorningAlarm);
#endif
  }

  void loop() {
    Alarm.delay(0);
  }
}
