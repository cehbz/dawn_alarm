#include <TimeAlarms.h>
// #include <tzfile.h>

#include "alarmer.h"
#include "fade.h"

namespace alarmer {
  static const ColorAtTime segments[] = {
    ColorAtTime(0.42, CRGB(14, 20, 28)),
    ColorAtTime(0.71, CRGB(28, 28, 44)),
    ColorAtTime(0.86, CRGB(62, 40, 9)),
    ColorAtTime(1.00, CRGB(128, 105, 66)),
  };
  static const int num_segs = sizeof(segments)/sizeof(segments[0]);
  static Fader fader(segments, num_segs, 1*SECS_PER_HOUR*1000);

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
    leds::setAnimator(fader);
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
