#include "postOptionsHandler.h"
#include <ArduinoJson.h>
#include "options.h"

void PostOptionsHandler::Handle(const char* path) {
  const size_t bufferSize = JSON_OBJECT_SIZE(7) + 110;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonObject& root = jsonBuffer.parseObject(*(client.client));
  if (!root.success()) {
    Serial.printf("parse failed\n");
    client.send400();
    return;
  }

  Serial.printf("@%lu", millis());
  Serial.print(", root[debug_alarmer] = "); root["debug_alarmer"].printTo(Serial);
  Serial.print(", root[debug_fade] = "); root["debug_fade"].printTo(Serial);
  Serial.print(", root[debug_http] = "); root["debug_http"].printTo(Serial);
  Serial.print(", root[debug_leds] = "); root["debug_leds"].printTo(Serial);
  Serial.print(", root[show_fps] = "); root["show_fps"].printTo(Serial);
  Serial.print(", root[debug_speedup] = "); root["debug_speedup"].printTo(Serial);
  Serial.println();

  options::debug_alarmer = root["debug_alarmer"];
  options::debug_fade = root["debug_fade"];
  options::debug_http = root["debug_http"];
  options::debug_leds = root["debug_leds"];
  options::show_fps = root["show_fps"];
  options::debug_speedup = root["debug_speedup"];
  client.send200();

  Serial.printf("@%lu", millis());
  Serial.printf(", debug_alarmer = %d", options::debug_alarmer);
  Serial.printf(", debug_fade = %d", options::debug_fade);
  Serial.printf(", debug_http = %d", options::debug_http);
  Serial.printf(", debug_leds = %d", options::debug_leds);
  Serial.printf(", show_fps = %d", options::show_fps);
  Serial.printf(", debug_speedup = %d", options::debug_speedup);
  Serial.println();
};
