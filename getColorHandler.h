#pragma once

#include "server.h"

class GetColorHandler : public RequestHandler {
public:
  GetColorHandler(WiFiClient* c) : RequestHandler(c) {};
  void Handle(const char*);
};
