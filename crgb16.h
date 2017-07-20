#pragma once

#include "leds.h"

// Representation of a 16 bit RGB pixel (Red, Green, Blue)
struct CRGB16 {
  union {
    struct {
      union {
        uint16_t r;
        uint16_t red;
      };
      union {
        uint16_t g;
        uint16_t green;
      };
      union {
        uint16_t b;
        uint16_t blue;
      };
    };
    uint16_t raw[3];
  };

  // default values are UNINITIALIZED
  inline CRGB16() __attribute__((always_inline))
  {
  }

  // allow construction from 16 bit R, G, B
  inline CRGB16( uint16_t ir, uint16_t ig, uint16_t ib)  __attribute__((always_inline))
    : r(ir), g(ig), b(ib)
  {
  }

  // // allow construction from 8 bit R, G, B
  // inline CRGB16( uint8_t ir, uint8_t ig, uint8_t ib)  __attribute__((always_inline))
  //   : r(ir<<8), g(ig<<8), b(ib<<8)
  // {
  // }

  // allow copy construction
  inline CRGB16(const CRGB16& rhs) __attribute__((always_inline))
    : r(rhs.r), g(rhs.g), b(rhs.b)
  {
  }

  // allow construction from 8 bit CRGB
  inline CRGB16(const CRGB& rhs) __attribute__((always_inline))
    : r(rhs.R<<8), g(rhs.G<<8), b(rhs.B<<8)
  {
  }

  // allow assignment from 8 bit CRGB
  inline CRGB16& operator= (const CRGB& rhs) __attribute__((always_inline))
  {
    r = rhs.R << 8;
    g = rhs.G << 8;
    b = rhs.B << 8;
    return *this;
  }

  __attribute__((always_inline)) static inline uint16_t qadd16(uint16_t i, uint16_t j) {
    unsigned int t = i + j;
    if( t > 65535) t = 65535;
    return t;
  }

  // convert CRGB16 to CRGB 8 bit
  inline CRGB CRGB16to8() const __attribute__((always_inline))
  {
    return CRGB(
                qadd16(r,0x80)>>8,
                qadd16(g,0x80)>>8,
                qadd16(b,0x80)>>8
                );
  }
};
