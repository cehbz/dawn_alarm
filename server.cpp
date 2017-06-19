#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include "server.h"
#include "leds.h"

namespace server {
  WiFiServer server(80);

  void setup() {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.printf("@%u: PANIC connect WiFi before starting server!\n", millis());
      panic();
    }
    if (!MDNS.begin("dawn")) {
      Serial.printf("@%u: PANIC Error setting up MDNS responder!\n", millis());
      panic();
    }
    Serial.printf("@%u: mDNS responder started\n", millis());
    server.begin();
    Serial.printf("@%u: HTTP server started\n", millis());
    MDNS.addService("http", "tcp", 80);
  }

  WiFiClient client;
  uint32_t clientConnectMillis = 0;
  static const uint32_t clientConnectTimeoutMillis = 10;
  static const uint32_t clientAvailableTimeoutMillis = 1000;

  void sendCommon(const char* s) {
    client.print("HTTP/1.1 ");
    client.println(s);
    client.println("Content-Type: text/html");
    client.println("Connection: close");  // the connection will be closed after completion of the response 
  }

  void send200() {
    sendCommon("200 OK");
  }

  void sendResponse() {
    send200();
    client.println();
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    CRGB16 color = leds::getColor();
    client.printf("Current color is %04x %04x %04x", color.r, color.g, color.b);
    client.println("</html>");
  }

  void send404() {
    sendCommon("404 Not Found");
  }

  void send400() {
    sendCommon("400 Bad Request");
  }

  void doPost(String path) {
    const size_t bufferSize = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(3) + 30;
    DynamicJsonBuffer jsonBuffer(bufferSize);

    JsonObject& root = jsonBuffer.parseObject(client);

    JsonObject& color = root["color"];
    const char* color_r = color["r"];
    const char* color_g = color["g"];
    const char* color_b = color["b"];

    Serial.printf("@%u: color %s %s %s\n", millis(), color_r, color_g, color_b);
    if (strlen(color_r) != 4 || strlen(color_g) != 4 || strlen(color_b) != 4) {
      send400();
      return;
    }
    uint16_t r = strtoul(color_r, 0, 16);
    uint16_t g = strtoul(color_g, 0, 16);
    uint16_t b = strtoul(color_b, 0, 16);
    Serial.printf("@%u: color %04x %04x %04x\n", millis(), r, g, b);
    leds::setColor(CRGB16(r, g, b));
    send200();
  }

  void doGet(String path) {
    if (path == "/") {
      sendResponse();
      return;
    }
    send404();
  }

  void loop() {
    // wait for a client
    if (!client) {
      client = server.available();
      if (!client) return;
      clientConnectMillis = millis();
      Serial.printf("@%u: new client\n", millis());
    }

    // wait for client to connect (establish connection)
    if (!client.connected()) {
      if (millis() - clientConnectMillis > clientConnectTimeoutMillis) {
        client.stop();
        Serial.printf("@%u: client timed out waiting to connect\n", millis());
      }
      return;
    }

    // wait for client to send data
    if (!client.available()) {
      if (millis() - clientConnectMillis > clientAvailableTimeoutMillis) {
        client.stop();
        Serial.printf("@%u: client timed out waiting for data\n", millis());
      }
      return;
    }

    String req = client.readStringUntil('\n');
    req.trim();
    Serial.printf("@%u: got request\n", millis());
    Serial.println(req);
    String s;
    do {
      s = client.readStringUntil('\n');
      s.trim();
      Serial.println(s);
    } while (s != "");

    // First line of HTTP request looks like "GET /path HTTP/1.1"
    // Retrieve the "/path" part by finding the spaces
    int path_start = req.indexOf(' ');
    int path_end = req.indexOf(' ', path_start + 1);
    if (path_start == -1) {
      Serial.printf("@%u: Invalid request\n", millis());
      return;
    }
    if (path_end == -1) {
      Serial.printf("@%u: Invalid path\n", millis());
      return;
    }
    String request = req.substring(0, path_start).c_str();
    String path = req.substring(path_start+1, path_end);
    if (request == "GET") {
      doGet(path);
    } else if (request == "POST") {
      doPost(path);
    } else {
      Serial.printf("@%u: unknown request\n", millis());
    };
    client.flush();

    Serial.printf("@%u: finished request\n", millis());
    client.stop();
  }
}
