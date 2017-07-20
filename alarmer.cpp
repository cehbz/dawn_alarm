#include <TimeAlarms.h>
// #include <tzfile.h>

#include "alarmer.h"
#include "fade.h"

namespace alarmer {
  // CRGB16(0x1000, 0x0400, 0x0080)
  static const fade::ColorAtTime segments[] = {
    fade::ColorAtTime(3.0/7.0, CRGB(14, 20, 28)),
    fade::ColorAtTime(5.0/7.0, CRGB(28, 28, 44)),
    fade::ColorAtTime(6.0/7.0, CRGB(62, 40, 9)),
    fade::ColorAtTime(7.0/7.0, CRGB(128, 105, 66)),
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
      DEBUG_ALARMER_PRINT(
        "segments[%d] %3d/1000, #%02x%02x%02x\n",
        i,
        int(s.pos*1000),
        s.color.R,
        s.color.G,
        s.color.B);
    }
    DEBUG_ALARMER_PRINT("duration %d\n", 1*SECS_PER_HOUR*1000);
#endif
    hwLeds::setAnimator(fader);
  }

  AlarmID_t dowAlarms[dowSaturday];
  void SetAlarm(const timeDayOfWeek_t dow, const int h, const int m) {
    Alarm.free(dowAlarms[dow]);
    dowAlarms[dow] = Alarm.alarmRepeat(dow, h, m, 0, MorningAlarm);
  }

  void setup() {
#ifdef DEBUG_ALARMER
    Alarm.timerOnce(1, MorningAlarm);
#else
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
#endif
  }

  void loop() {
    Alarm.delay(0);
  }
}
