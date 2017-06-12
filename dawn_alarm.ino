#include "ntp.h"
#include "leds.h"

#include <TimeAlarms.h>

void MorningAlarm() {
  Serial.println("MorningAlarm");
#ifndef DEBUG
  leds::start(1*SECS_PER_HOUR*1000);
#else
  leds::start(1*SECS_PER_MIN*1000);
#endif
}

void setup() {
  Serial.begin(115200);
  delay(250);
  ntp::setup();
  leds::setup();
#ifndef DEBUG
  Alarm.alarmRepeat(6,0,0, MorningAlarm);
#else
  Alarm.alarmOnce(hour(now()), minute(now()), second(now())+5, MorningAlarm);
#endif
}

void loop() {
  ntp::loop();
  leds::loop();
  Alarm.delay(0);
}
