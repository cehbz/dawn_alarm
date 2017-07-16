#pragma once

#include "server.h"

class PostInterpolateHandler : public RequestHandler {
public:
  PostInterpolateHandler(WiFiClient* c) : RequestHandler(c) {};
  void Handle(const char*);
};
