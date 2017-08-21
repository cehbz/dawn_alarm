#pragma once

#include <TimeAlarms.h>

namespace alarmer {
  extern const char* const dayNames[];
  extern const char* alarmPeriodNames[];

  void MorningAlarm();
  void SetAlarm(const timeDayOfWeek_t DOW, const int h, const int m);
  void setup();
  void loop();
}
