#pragma once

#include "server.h"

class PostAlarmsHandler : public RequestHandler {
public:
  PostAlarmsHandler(WiFiClient* c) : RequestHandler(c) {};
  void Handle(const char*);
};
