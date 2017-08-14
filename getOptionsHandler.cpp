#include "getOptionsHandler.h"
#include <ArduinoJson.h>

#include "options.h"
#include "leds.h"

void GetOptionsHandler::Handle(const char* path) {
  const size_t bufferSize = JSON_OBJECT_SIZE(6) + 95;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonObject& root = jsonBuffer.createObject();
  root["debug_alarmer"] = options::debug_alarmer;
  root["debug_fade"] = options::debug_fade;
  root["debug_http"] = options::debug_http;
  root["debug_leds"] = options::debug_leds;
  root["num_leds"] = leds::NUM_LEDS;
  root["show_fps"] = options::show_fps;
  client.sendJSON(root);
};
