#include "getAlarmsHandler.h"
#include <ArduinoJson.h>
#include "alarmer.h"
#include "options.h"

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
    if (options::debug_http) Serial.printf("@%lu: alarms[%d] %s", millis(), i, alarmer::alarmPeriodNames[Alarm.readType(i)]);
    if (Alarm.readType(i) != dtWeeklyAlarm) {
      if (options::debug_http) Serial.println();
      continue;
    }
    auto a = Alarm.read(i);
    auto dow = timeDayOfWeek_t((dayOfWeek(a)+2)%7+1);
    if (options::debug_http) Serial.printf(", %s %02lu:%02lu (%2lu, %lu)\n",
                  alarmer::dayNames[dow-1], numberOfHours(a), numberOfMinutes(a),
                  a, dow);
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
    if (options::debug_http) Serial.printf("@%lu: [%d]", millis(), d);
    timeDayOfWeek_t dow = timeDayOfWeek_t(d);
    if (!alarms[dow-1]) {
      Serial.printf("missing alarm for %s\n", alarmer::dayNames[dow-1]);
      client.send500();
      return;
    }
    if (options::debug_http) Serial.printf(" %s %s\n", alarmer::dayNames[dow-1], root[alarmer::dayNames[dow-1]].as<const char *>());
  }
  if (options::debug_http) Serial.println();
  client.sendJSON(root);
};
