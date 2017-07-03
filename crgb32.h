#pragma once

#include <FastLED.h>

// Representation of a 32 bit RGB pixel (Red, Green, Blue)
struct CRGB32 {
  union {
    struct {
      union {
        uint32_t r;
        uint32_t red;
      };
      union {
        uint32_t g;
        uint32_t green;
      };
      union {
        uint32_t b;
        uint32_t blue;
      };
    };
    uint32_t raw[3];
  };

  // default values are UNINITIALIZED
  inline CRGB32() __attribute__((always_inline))
  {
  }

  // allow construction from 32 bit R, G, B
  inline CRGB32( uint32_t ir, uint32_t ig, uint32_t ib)  __attribute__((always_inline))
    : r(ir), g(ig), b(ib)
  {
  }

  // // allow construction from 8 bit R, G, B
  // inline CRGB32( uint8_t ir, uint8_t ig, uint8_t ib)  __attribute__((always_inline))
  //   : r(ir<<8), g(ig<<8), b(ib<<8)
  // {
  // }

  // allow copy construction
  inline CRGB32(const CRGB32& rhs) __attribute__((always_inline))
    : r(rhs.r), g(rhs.g), b(rhs.b)
  {
  }

  // allow construction from 8 bit CRGB
  inline CRGB32(const CRGB& rhs) __attribute__((always_inline))
    : r(rhs.r<<8), g(rhs.g<<8), b(rhs.b<<8)
  {
  }

  // allow assignment from 8 bit CRGB
  inline CRGB32& operator= (const CRGB& rhs) __attribute__((always_inline))
  {
    r = rhs.r << 8;
    g = rhs.g << 8;
    b = rhs.b << 8;
    return *this;
  }

  __attribute__((always_inline)) static inline uint32_t qadd32(uint32_t i, uint32_t j) {
    int64_t t = i + j;
    if( t  >  uint32_t(-1)) t = uint32_t(-1);
    return t;
  }

  // convert CRGB32 to CRGB 8 bit
  inline CRGB CRGB32to8() __attribute__((always_inline))
  {
    return CRGB(
                qadd32(r,1<<23)>>24,
                qadd32(g,1<<23)>>24,
                qadd32(b,1<<23)>>24
                );
  }

  // convert CRGB32 to CRGB 16 bit
  inline CRGB CRGB32to16() __attribute__((always_inline))
  {
    return CRGB(
                qadd32(r,1<<15)>>16,
                qadd32(g,1<<15)>>16,
                qadd32(b,1<<15)>>16
                );
  }
};
