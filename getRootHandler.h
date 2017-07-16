#pragma once
#include "server.h"

class GetRootHandler : public RequestHandler {
public:
  GetRootHandler(WiFiClient* c) : RequestHandler(c) {};
  void Handle(const char*);
};
