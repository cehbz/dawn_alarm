#include <TimeAlarms.h>

#include "options.h"
#include "alarmer.h"
#include "fade.h"

namespace alarmer {
  const char* const dayNames[] = {
    "sunday",
    "monday",
    "tuesday",
    "wednesday",
    "thursday",
    "friday",
    "saturday"
  };

  const char* alarmPeriodNames[] = {
    "NotAllocated",
    "Timer",
    "ExplicitAlarm",
    "DailyAlarm",
    "WeeklyAlarm",
    "LastAlarmType"
  };

  static const ColorAtTime segments[] = {
    ColorAtTime(0.40, CRGB(14, 20, 28)),
    ColorAtTime(0.65, CRGB(28, 28, 44)),
    ColorAtTime(0.80, CRGB(62, 40, 9)),
    ColorAtTime(0.85, CRGB(128, 105, 66)),
    ColorAtTime(1.00, CRGB(255, 210, 132)),
  };
  static const int num_segs = sizeof(segments)/sizeof(segments[0]);
  static Fader fader(segments, num_segs, 1*SECS_PER_HOUR*1000);

  void MorningAlarm() {
    Serial.println("MorningAlarm");
    fader.reset();
    Serial.printf("@%lu ", millis()); fader.print(); Serial.println();
    if (options::debug_alarmer) {
      for (int i = 0; i < num_segs; i++) {
        auto s = segments[i];
        Serial.printf(
                      "segments[%d] %3d/1000, #%02x%02x%02x\n",
                      i,
                      int(s.pos*1000),
                      s.color.R,
                      s.color.G,
                      s.color.B);
      }
      Serial.printf("duration %d\n", 1*SECS_PER_HOUR*1000);
    }
    leds::setAnimator(fader);
  }

  AlarmID_t dowAlarms[dowSaturday];
  void SetAlarm(const timeDayOfWeek_t dow, const int h, const int m) {
    Alarm.free(dowAlarms[dow]);
    dowAlarms[dow] = Alarm.alarmRepeat(dow, h, m, 0, MorningAlarm);
  }

    void setup() {
    if (options::debug_alarmer) {
      MorningAlarm();
      return;
    }
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
  }

  void loop() {
    Alarm.delay(0);
  }
}
