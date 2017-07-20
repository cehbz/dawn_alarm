#pragma once

#include <NeoPixelBus.h>

#define neopixelbus

typedef RgbwColor CRGB;

class Animator {
public:
  virtual void render() = 0;
  virtual void print() = 0;
};

class hwLeds {
public:
#ifdef neopixelbus
  static const int NUM_LEDS = 60; // Adafruit neopixel ring
#endif
#ifdef fastled
  static const int NUM_LEDS = 30; // WS2812B
  // static const int NUM_LEDS = 60; // APA102
#endif
#ifdef pixie
  static const int NUM_LEDS = 3; // Adafruit Pixie
#endif
  static const CRGB getColor();
  static const CRGB* getColors();
  static void setColor(const CRGB& color);
  static void setColors(const CRGB* leds);
  static void setAnimator(Animator& animator);

  static void setup();
  static void loop();
};
