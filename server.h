#pragma once
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "options.h"

class HTTPClient {
public:
  WiFiClient* client;

  void sendCommon(const char* s) {
    client->print("HTTP/1.1 ");
    client->println(s);
    client->println("Access-Control-Allow-Origin: *");
    client->println("Connection: close");  // the connection will be closed after completion of the response
  }

  void send200() { sendCommon("200 OK"); }
  void send404() { sendCommon("404 Not Found"); }
  void send400() { sendCommon("400 Bad Request"); }
  void send500() { sendCommon("500 Internal Server Error"); }

  void sendJSON() {
    send200();
    client->println("Content-Type: application/json");
    client->println();
  }

  void sendJSON(JsonObject& root) {
    if (options::debug_http) {
      Serial.printf("@%lu: sendJSON(", millis()); root.printTo(Serial); Serial.println(")"); Serial.flush();
    }
    sendJSON();
    root.printTo(*client);
  }

  void sendJSON(JsonArray& root) {
    if (options::debug_http) {
      Serial.printf("@%lu: sendJSON(", millis()); root.printTo(Serial); Serial.println(")"); Serial.flush();
    }
    sendJSON();
    root.printTo(*client);
  }

  void sendHTML(const char* text) {
    send200();
    client->println("Content-Type: text/html");
    client->println();
    client->println("<!DOCTYPE HTML>");
    client->println("<html>");
    client->print(text);
    client->println("</html>");
  }

  void println(const char* text) {
    client->println(text);
  }

  HTTPClient(WiFiClient* c) : client{c} {};
};

class RequestHandler {
protected:
  HTTPClient client;
public:
  RequestHandler(WiFiClient* c) : client{c} {};
  virtual void Handle(const char* path) = 0;
};

namespace HTTPserver {
  void setup();
  void loop();
  void addRoute(const char *method, const char* path, const RequestHandler& r);
};
