#pragma once

#include "common.h"

template <uint8_t T>
class Amp {
  static uint8_t m_expression;

public:
  INLINE static void initialize() {
    m_expression = 128;
  }

  INLINE static void set_expression(uint8_t controller_value) {
    if (controller_value == 127) {
      m_expression = 128;
    } else {
      m_expression = high_byte(((controller_value + 1) << 1) * ((controller_value + 1) << 1)) >> 1;
    }
  }

  INLINE static void set_volume_exp_amt(uint8_t controller_value) {
    // TODO
  }

  INLINE static int8_t clock(int8_t audio_input, uint8_t gain_control) {
    uint8_t gain;
    if (m_expression == 128) {
      gain = gain_control;
    } else {
      gain = high_byte(gain_control * (m_expression << 1));
    }

    if (gain == 128) {
      return audio_input;
    }
    return high_sbyte(audio_input * (gain << 1));
  }
};

template <uint8_t T> uint8_t  Amp<T>::m_expression;
