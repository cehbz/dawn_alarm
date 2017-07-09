#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include "leds.h"
#include "server.h"
#include "singleColor.h"
#include "singleFramer.h"
#include "interpolate.h"

namespace server {
  WiFiServer server(80);

  void registerDynamicDNS() {
    // Use WiFiClientSecure class to create TLS connection
    WiFiClientSecure secureClient;
    const char *host = "dynamicdns.park-your-domain.com";
    const char* fingerprint = "8A BD E4 A3 CE 85 A3 FB 94 71 B2 05 3D C6 78 7A 4D 55 63 8B";
    Serial.print("GET https://"); Serial.print(host);
    if (!secureClient.connect(host, 443)) {
      Serial.println("\nconnection failed");
      return;
    }

    if (!secureClient.verify(fingerprint, host)) {
      Serial.println("\ncertificate doesn't match");
      return;
    }

    const char* url = "/update?host=dawn&domain=ceh.bz&password=90abd539fc424917a3cae3d285d8d791&ip=";
    Serial.print(url); Serial.print(WiFi.localIP());

    secureClient.print("GET "); secureClient.print(url); secureClient.print(WiFi.localIP()); secureClient.print(" HTTP/1.1\r\n");
    secureClient.print("Host: "); secureClient.print(host); secureClient.print("\r\n");
    secureClient.print("User-Agent: DynDNSClientESP8266\r\n");
    secureClient.print("Connection: close\r\n\r\n");
    secureClient.flush();

    Serial.println();
    do {
      Serial.println(secureClient.readStringUntil('\n'));
    } while (secureClient.connected());
    secureClient.stop();
    Serial.println("dawn.ceh.bz registered");
  }

  void setup() {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.printf("@%lu: PANIC connect WiFi before starting server!\n", millis());
      panic();
    }
    if (!MDNS.begin("dawn")) {
      Serial.printf("@%lu: PANIC Error setting up MDNS responder!\n", millis());
      panic();
    }
    Serial.printf("@%lu: mDNS responder started\n", millis());
    registerDynamicDNS();
    server.begin();
    Serial.printf("@%lu: HTTP server started\n", millis());
    MDNS.addService("http", "tcp", 80);
  }

  WiFiClient client;
  uint32_t clientConnectMillis = 0;
  static const uint32_t clientConnectTimeoutMillis = 10;
  static const uint32_t clientAvailableTimeoutMillis = 1000;

  void sendCommon(const char* s) {
    client.print("HTTP/1.1 ");
    client.println(s);
    client.println("Access-Control-Allow-Origin: *");
    client.println("Connection: close");  // the connection will be closed after completion of the response
  }

  void send200() {
    sendCommon("200 OK");
  }

  void getRoot() {
    send200();
    client.println("Content-Type: text/html");
    client.println();
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    CRGB color = leds::getColor();
    client.printf("Current color is %u %u %u", color.r, color.g, color.b);
    client.println("</html>");
  }

  void getColor() {
    const CRGB color = leds::getColor();
    const size_t bufferSize = JSON_OBJECT_SIZE(3) + 30;
    DynamicJsonBuffer jsonBuffer(bufferSize);
    JsonObject& root = jsonBuffer.createObject();
    root["r"]=color.r;
    root["g"]=color.g;
    root["b"]=color.b;
    send200();
    client.println("Content-Type: application/json");
    client.println();
    root.printTo(client);
  }

  void getColors() {
    const CRGB* colors = leds::getColors();
    const size_t bufferSize = JSON_ARRAY_SIZE(leds::NUM_LEDS) + leds::NUM_LEDS*JSON_OBJECT_SIZE(3) + 30*leds::NUM_LEDS;
    DynamicJsonBuffer jsonBuffer(bufferSize);

    JsonArray& root = jsonBuffer.createArray();
    for (int i = 0; i < leds::NUM_LEDS; i++) {
      JsonObject& led = root.createNestedObject();
      led["r"]=colors[i].r;
      led["g"]=colors[i].g;
      led["b"]=colors[i].b;
    }
    send200();
    client.println("Content-Type: application/json");
    client.println();
    root.printTo(client);
  }

  void send404() {
    sendCommon("404 Not Found");
  }

  void send400() {
    sendCommon("400 Bad Request");
  }

  static singleColor::Monochromer monochromer(CRGB(0,0,0));
  void doPostColor(String path) {
    const size_t bufferSize = JSON_OBJECT_SIZE(3) + 30;
    DynamicJsonBuffer jsonBuffer(bufferSize);

    JsonObject& root = jsonBuffer.parseObject(client);
    if (!root.success()) {
      Serial.printf("parse failed\n");
      send400();
      return;
    }

    const CRGB c(root["r"], root["g"], root["b"]);
    Serial.printf("@%lu: color %u [%02x], %u [%02x], %u [%02x]\n", millis(), c.r, c.r, c.g, c.g, c.b, c.b);
    monochromer = singleColor::Monochromer(c);
    leds::setAnimator(monochromer);
    send200();
  }

  static CRGB leds[leds::NUM_LEDS];
  static SingleFramer singleFramer(leds);
  void doPostColors(String path) {
    const size_t bufferSize = JSON_ARRAY_SIZE(leds::NUM_LEDS) + leds::NUM_LEDS*JSON_OBJECT_SIZE(3) + 30*leds::NUM_LEDS;
    DynamicJsonBuffer jsonBuffer(bufferSize);

    JsonArray& root = jsonBuffer.parseArray(client);
    if (!root.success()) {
      Serial.printf("parse failed\n");
      send400();
      return;
    }

    for (int i = 0; i < leds::NUM_LEDS; i++) {
      const JsonObject& r = root[i];
      const CRGB c(r["r"], r["g"], r["b"]);
      leds[i] = c;
      Serial.printf("@%lu: leds[%2d] %u [%02x], %u [%02x], %u [%02x]\n", millis(), i, leds[i].r, leds[i].r, leds[i].g, leds[i].g, leds[i].b, leds[i].b);
    }

    leds::setAnimator(singleFramer);
    send200();
  }

  static interpolate::Interpolater interpolater(CRGB(0,0,0),CRGB(0,0,0));
  void doPostInterpolate(String path) {
    const size_t bufferSize = JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(3) + 70;
    DynamicJsonBuffer jsonBuffer(bufferSize);

    JsonObject& root = jsonBuffer.parseObject(client);
    if (!root.success()) {
      Serial.printf("parse failed\n");
      send400();
      return;
    }

    CRGB start(root["start"]["r"], root["start"]["g"], root["start"]["b"]);
    CRGB end(root["end"]["r"], root["end"]["g"], root["end"]["b"]);

    Serial.printf("@%lu: start color %u [%02x], %u [%02x], %u [%02x]\n", millis(), start.r, start.r, start.g, start.g, start.b, start.b);
    Serial.printf("@%lu: end color %u [%02x], %u [%02x], %u [%02x]\n", millis(), end.r, end.r, end.g, end.g, end.b, end.b);
    interpolater = interpolate::Interpolater(start, end);
    leds::setAnimator(interpolater);
    send200();
  }

  void doPost(String path) {
    if (path == "/color") {
      doPostColor(path);
      return;
    }
    if (path == "/colors") {
      doPostColors(path);
      return;
    }
    if (path == "/interpolate") {
      doPostInterpolate(path);
      return;
    }
    send404();
  }

  void doOptions(String path) {
    send200();
    client.println("Access-Control-Allow-Methods: POST, GET, OPTIONS");
    client.println("Access-Control-Allow-Headers: content-type");
  }

  void doGet(String path) {
    if (path == "/color") {
      getColor();
      return;
    }
    if (path == "/colors") {
      getColors();
      return;
    }
    if (path == "/") {
      getRoot();
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
      Serial.printf("@%lu: new client\n", millis());
    }

    // wait for client to connect (establish connection)
    if (!client.connected()) {
      if (millis() - clientConnectMillis > clientConnectTimeoutMillis) {
        client.stop();
        Serial.printf("@%lu: client timed out waiting to connect\n", millis());
      }
      return;
    }

    // wait for client to send data
    if (!client.available()) {
      if (millis() - clientConnectMillis > clientAvailableTimeoutMillis) {
        client.stop();
        Serial.printf("@%lu: client timed out waiting for data\n", millis());
      }
      return;
    }

    String req = client.readStringUntil('\n');
    req.trim();
    Serial.printf("@%lu: got request\n", millis());
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
      Serial.printf("@%lu: Invalid request\n", millis());
      return;
    }
    if (path_end == -1) {
      Serial.printf("@%lu: Invalid path\n", millis());
      return;
    }
    String request = req.substring(0, path_start).c_str();
    String path = req.substring(path_start+1, path_end);
    if (request == "GET") {
      doGet(path);
    } else if (request == "POST") {
      doPost(path);
    } else if (request == "OPTIONS") {
      doOptions(path);
    } else {
      Serial.printf("@%lu: unknown request\n", millis());
    };
    client.flush();

    Serial.printf("@%lu: finished request\n", millis());
    client.stop();
  }
}
