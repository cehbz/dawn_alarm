#include "ntp.h"
#include "leds.h"

#include <TimeAlarms.h>

void MorningAlarm() {
  Serial.println("MorningAlarm");
  leds::start(1*SECS_PER_MIN*1000);
}

void setup() {
  Serial.begin(115200);
  delay(250);
  ntp::setup();
  leds::setup();
  // Alarm.alarmRepeat(6,0,0, MorningAlarm);
  Alarm.alarmOnce(hour(now()), minute(now()), second(now())+5, MorningAlarm);
}

void loop() {
  ntp::loop();
  leds::loop();
  Alarm.delay(0);
}
