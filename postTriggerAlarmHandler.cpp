#include "postTriggerAlarmHandler.h"
#include <ArduinoJson.h>

#include "alarmer.h"

void PostTriggerAlarmHandler::Handle(const char* path) {
  Serial.printf("@%lu: triggerAlarm\n", millis());
  alarmer::MorningAlarm();
  client.send200();
};
