#include "postAlarmsHandler.h"
#include <ArduinoJson.h>
#include "alarmer.h"

int s2toi(const char* s) {
  // returns a two digit integer pointed to by s
  // returns -1 if error
  if (s[0] < '0' || s[0] > '9' || s[1] < '0' || s[1] > '9') return -1;
  return (s[0]-'0')*10+s[1]-'0';
}

void PostAlarmsHandler::Handle(const char* path) {
  const size_t bufferSize = JSON_OBJECT_SIZE(7) + 110;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonObject& root = jsonBuffer.parseObject(*(client.client));
  if (!root.success()) {
    Serial.printf("parse failed\n");
    client.send400();
    return;
  }
  for ( int d = dowSunday; d <= dowSaturday; d++) {
    timeDayOfWeek_t dow = timeDayOfWeek_t(d);
    const char *c = root[alarmer::dayNames[dow-1]];
    if (c == NULL) {
      Serial.printf("missing alarm for %s\n", alarmer::dayNames[dow-1]);
      client.send400();
      return;
    }
    Serial.printf("%s: %s\n", alarmer::dayNames[dow-1], c);
    if (strlen(c) != 5 || c[2] != ':') {
      Serial.printf("bad alarm time for %s: %s. strlen = %d, c[2] = %c\n", alarmer::dayNames[dow-1], c, strlen(c), c[2]);
      client.send400();
      return;
    }
    int h = s2toi(c);
    int m = s2toi(&c[3]);
    if (h < 0 || h > 23 || m < 0 || m > 59) {
      Serial.printf("bad alarm time for %s: %s. h %d, m %d\n", alarmer::dayNames[dow-1], c, h, m);
      client.send400();
      return;
    }
  }
  for ( int d = dowSunday; d <= dowSaturday; d++) {
    timeDayOfWeek_t dow = timeDayOfWeek_t(d);
    const char *c = root[alarmer::dayNames[dow-1]];
    int h = s2toi(c);
    int m = s2toi(&c[3]);
    alarmer::SetAlarm(dow, h, m);
  }
  client.send200();
}
