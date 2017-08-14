#pragma once

#include "server.h"

class GetOptionsHandler : public RequestHandler {
public:
  GetOptionsHandler(WiFiClient* c) : RequestHandler(c) {};
  void Handle(const char*);
};
