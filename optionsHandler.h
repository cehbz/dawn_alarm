#pragma once

#include "server.h"

class OptionsHandler : public RequestHandler {
public:
  OptionsHandler(WiFiClient* c) : RequestHandler(c) {};
  void Handle(const char*);
};
