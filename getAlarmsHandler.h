#pragma once

#include "server.h"

class GetAlarmsHandler : public RequestHandler {
public:
  GetAlarmsHandler(WiFiClient* c) : RequestHandler(c) {};
  void Handle(const char*);
};
