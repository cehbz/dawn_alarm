#include "getAlarmsHandler.h"
#include <ArduinoJson.h>
#include "alarmer.h"

void GetAlarmsHandler::Handle(const char* path) {
  const size_t bufferSize = JSON_OBJECT_SIZE(7) + 110;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonObject& root = jsonBuffer.createObject();
  bool alarms[dowSaturday];
  typedef char AlarmTimeStr[6];
  AlarmTimeStr alarmTimes[7];
  for ( int d = dowSunday; d <= dowSaturday; d++) {
    timeDayOfWeek_t dow = timeDayOfWeek_t(d);
    alarms[dow] = false;
  }
  for (int i = 0; i < dtNBR_ALARMS; i++) {
    Serial.printf("%d: %d\n", i, Alarm.readType(i));
    if (Alarm.readType(i) != dtWeeklyAlarm) continue;
    auto a = Alarm.read(i);
    Serial.printf("%2lu: %lu %02lu:%02lu", a, (dayOfWeek(a)+2)%7+1, numberOfHours(a), numberOfMinutes(a)); Serial.flush();
    auto dow = timeDayOfWeek_t((dayOfWeek(a)+2)%7+1);
    Serial.printf(" %s\n", alarmer::dayNames[dow-1]); Serial.flush();
    if (alarms[dow]) {
      Serial.printf("duplicate alarm for %s\n", alarmer::dayNames[dow-1]);
      client.send500();
      return;
    }
    alarms[dow] = true;
    sprintf(alarmTimes[dow-1], "%02lu:%02lu", numberOfHours(a), numberOfMinutes(a));
    root[alarmer::dayNames[dow-1]] = alarmTimes[dow-1];
  }
  for ( int d = dowSunday; d <= dowSaturday; d++) {
    timeDayOfWeek_t dow = timeDayOfWeek_t(d);
    if (!alarms[dow-1]) {
      Serial.printf("missing alarm for %s\n", alarmer::dayNames[dow-1]);
      client.send500();
      return;
    }
    Serial.printf("\"%s\": \"%s\"\n", alarmer::dayNames[dow-1], root[alarmer::dayNames[dow-1]].as<const char *>());
  }
  client.sendJSON(root);
};
