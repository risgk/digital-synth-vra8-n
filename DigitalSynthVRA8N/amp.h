#pragma once

#include "common.h"

template <uint8_t T>
class Amp {
  static uint8_t m_expression;

public:
  INLINE static void initialize() {
    m_expression = 252;
  }

  INLINE static void set_expression(uint8_t controller_value) {
    m_expression = high_byte((controller_value << 1) * (controller_value << 1));
  }

  INLINE static void set_volume_exp_amt(uint8_t controller_value) {
    // TODO
  }

  INLINE static int16_t clock(int16_t audio_input, uint8_t gain_control) {
    uint8_t gain = high_byte(gain_control * m_expression) << 1;
    return mul_q15_q8(audio_input, gain);
  }
};

template <uint8_t T> uint8_t  Amp<T>::m_expression;
