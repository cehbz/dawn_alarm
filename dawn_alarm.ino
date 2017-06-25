#include "dawn_alarm.h"
#include "ntp.h"
#include "leds.h"
#include "server.h"

#include <TimeAlarms.h>

void MorningAlarm() {
  Serial.println("MorningAlarm");
  leds::start(1*SECS_PER_HOUR*1000);
}

void setup() {
  Serial.begin(115200);
  delay(250);
  ntp::setup();
  leds::setup();
  server::setup();
#ifndef DEBUG_DAWN
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
