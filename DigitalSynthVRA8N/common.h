#pragma once

#define DEBUG

#define SERIAL_SPEED (38400)    // for Serial MIDI
//#define SERIAL_SPEED (31250)    // for MIDI Shield

#define PITCH_BEND_MINUS_RANGE (12)    // Max: 24
#define PITCH_BEND_PLUS_RANGE  (12)    // Max: 24
#define TRANSPOSE              (+0)    // Min: -24, Max: +24
#define FLUCTUATION_INIT       (127)   // Min: 0, Max: 127

#include "constants.h"

#define INLINE inline __attribute__((always_inline))

INLINE uint8_t low_byte(uint16_t x) {
  return x & 0xFF;
}

INLINE uint8_t high_byte(uint16_t x) {
  return x >> 8;
}

INLINE int8_t high_sbyte(int16_t x) {
  return x >> 8;
}

INLINE uint8_t hhigh_byte(__uint24 x) {
  return x >> 16;
}
