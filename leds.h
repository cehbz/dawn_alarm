#pragma once

#include <NeoPixelBus.h>

#define neopixelbus

typedef RgbwColor CRGB;

class Animator {
public:
  virtual void render() = 0;
  virtual void print() = 0;
};

namespace leds {
#ifdef neopixelbus
  const int NUM_LEDS = 60; // Adafruit neopixel ring
#endif
#ifdef fastled
  const int NUM_LEDS = 30; // WS2812B
  // const int NUM_LEDS = 60; // APA102
#endif
#ifdef pixie
  const int NUM_LEDS = 3; // Adafruit Pixie
#endif
  const CRGB getColor();
  const CRGB* getColors();
  void setColor(const CRGB& color);
  void setColors(const CRGB* leds);
  void setAnimator(Animator& animator);

  void setup();
  void loop();
};
