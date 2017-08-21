#pragma once

#include "server.h"

class PostTriggerAlarmHandler : public RequestHandler {
public:
  PostTriggerAlarmHandler(WiFiClient* c) : RequestHandler(c) {};
  void Handle(const char*);
};
