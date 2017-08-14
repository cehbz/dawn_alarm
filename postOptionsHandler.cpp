#include "postOptionsHandler.h"
#include <ArduinoJson.h>
#include "options.h"

void PostOptionsHandler::Handle(const char* path) {
  const size_t bufferSize = JSON_OBJECT_SIZE(6) + 95;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonObject& root = jsonBuffer.parseObject(*(client.client));
  if (!root.success()) {
    Serial.printf("parse failed\n");
    client.send400();
    return;
  }
  Serial.printf("@%lu", millis());
  Serial.print(", debug_alarmer = "); root["debug_alarmer"].printTo(Serial);
  Serial.print(", debug_fade = "); root["debug_fade"].printTo(Serial);
  Serial.print(", debug_http = "); root["debug_http"].printTo(Serial);
  Serial.print(", debug_leds = "); root["debug_leds"].printTo(Serial);
  Serial.print(", show_fps = "); root["show_fps"].printTo(Serial);
  Serial.println();

  options::debug_alarmer = root["debug_alarmer"];
  options::debug_fade = root["debug_fade"];
  options::debug_http = root["debug_http"];
  options::debug_leds = root["debug_leds"];
  options::show_fps = root["show_fps"];
};
