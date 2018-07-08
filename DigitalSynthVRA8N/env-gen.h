#pragma once

#include "common.h"
#include "mul-q.h"

template <uint8_t T>
class EnvGen {
  static const uint8_t STATE_ATTACK  = 0;
  static const uint8_t STATE_SUSTAIN = 1;
  static const uint8_t STATE_IDLE    = 2;

  static uint8_t  m_state;
  static uint16_t m_level;
  static uint8_t  m_attack_update_coef;
  static uint8_t  m_decay_update_coef;
  static boolean  m_sustain;
  static uint8_t  m_rest;

public:
  INLINE static void initialize() {
    m_state = STATE_IDLE;
    m_level = 0;
    set_attack(0);
    set_decay(0);
    set_sustain(true);
  }

  INLINE static void set_attack(uint8_t controller_value) {
    m_attack_update_coef = controller_value + 2;
  }

  INLINE static void set_decay(uint8_t controller_value) {
    m_decay_update_coef = controller_value + 2;
  }

  INLINE static void set_sustain(boolean on) {
    m_sustain = on;
  }

  INLINE static void note_on() {
    m_state = STATE_ATTACK;
    m_rest = m_attack_update_coef;
  }

  INLINE static void note_off() {
    m_state = STATE_IDLE;
    m_rest = m_decay_update_coef;
  }

  INLINE static uint8_t clock(uint8_t count) {
    if ((count & (ENV_GEN_CONTROL_INTERVAL - 1)) == ((T == 0) ? 2 : 2)) {
      switch (m_state) {
      case STATE_ATTACK:
        m_rest--;
        if (m_rest == 0) {
          m_rest = m_attack_update_coef;
          if (m_level >= ENV_GEN_LEVEL_MAX) {
            m_level = ENV_GEN_LEVEL_MAX;
            m_state = STATE_SUSTAIN;
            m_rest = m_decay_update_coef;
          } else {
            m_level = ENV_GEN_LEVEL_MAX_X_1_5 - mul_q16_q8(ENV_GEN_LEVEL_MAX_X_1_5 - m_level,
                                                           188 + (m_attack_update_coef >> 1));
            if (m_level >= ENV_GEN_LEVEL_MAX) {
              m_level = ENV_GEN_LEVEL_MAX;
            }
          }
        }
        break;
      case STATE_SUSTAIN:
        m_level = ENV_GEN_LEVEL_MAX;
        if (!m_sustain) {
          m_state = STATE_IDLE;
          m_rest = m_decay_update_coef;
        }
        break;
      case STATE_IDLE:
        if (m_decay_update_coef == 0) {
          break;
        }
        m_rest--;
        if (m_rest == 0) {
          m_rest = m_decay_update_coef;
          if (m_level < ((T == 0) ? 0x0100 : 0x0400 /* gate for amp */)) {
            m_level = 0;
          } else {
            m_level = mul_q16_q8(m_level, 188 + (m_decay_update_coef >> 1));
          }
        }
        break;
      }
    }

    return high_byte(m_level);
  }
};

template <uint8_t T> uint8_t  EnvGen<T>::m_state;
template <uint8_t T> uint16_t EnvGen<T>::m_level;
template <uint8_t T> uint8_t  EnvGen<T>::m_attack_update_coef;
template <uint8_t T> uint8_t  EnvGen<T>::m_decay_update_coef;
template <uint8_t T> boolean  EnvGen<T>::m_sustain;
template <uint8_t T> uint8_t  EnvGen<T>::m_rest;
