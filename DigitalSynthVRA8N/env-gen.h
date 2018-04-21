#pragma once

#include "common.h"
#include "mul-q.h"

template <uint8_t T>
class EnvGen {
  static const uint8_t STATE_ATTACK  = 0;
  static const uint8_t STATE_SUSTAIN = 1;
  static const uint8_t STATE_IDLE    = 2;

  static const uint16_t ATTACK_STEP            = 0x4000;
  static const uint8_t ATTACK_UPDATE_INTERVAL  = 1;

  static uint8_t  m_state;
  static uint16_t m_level;
  static boolean  m_sustain;
  static uint8_t  m_decay_update_interval;
  static uint8_t  m_rest;

public:
  INLINE static void initialize() {
    m_state = STATE_IDLE;
    m_level = 0;
    set_sustain(true);
    set_decay(0);
  }

  INLINE static void set_attack(uint8_t controller_value) {
  }

  INLINE static void set_decay(uint8_t controller_value) {
    if (controller_value < 32) {
      m_decay_update_interval = (controller_value >> 1) + 1;
    } else {
      m_decay_update_interval = high_byte((controller_value << 1) *
                                          (controller_value << 1)) + 1;
    }
  }

  INLINE static void set_sustain(boolean on) {
    m_sustain = on;
  }

  INLINE static void note_on() {
    m_state = STATE_ATTACK;
    m_rest = ATTACK_UPDATE_INTERVAL;
  }

  INLINE static void note_off() {
    m_state = STATE_IDLE;
    m_rest = m_decay_update_interval;
  }

  INLINE static uint8_t clock(uint8_t count) {
    if ((count & (ENV_GEN_CONTROL_INTERVAL - 1)) == ((T == 0) ? 2 : 2)) {
      switch (m_state) {
      case STATE_ATTACK:
        m_rest--;
        if (m_rest == 0) {
          m_rest = ATTACK_UPDATE_INTERVAL;
          if (m_level >= ENV_GEN_LEVEL_MAX - ATTACK_STEP) {
            m_level = ENV_GEN_LEVEL_MAX;
            m_state = STATE_SUSTAIN;
            m_rest = m_decay_update_interval;
          } else {
            m_level += ATTACK_STEP;
          }
        }
        break;
      case STATE_SUSTAIN:
        m_level = ENV_GEN_LEVEL_MAX;
        if (!m_sustain) {
          m_state = STATE_IDLE;
          m_rest = m_decay_update_interval;
        }
        break;
      case STATE_IDLE:
        m_rest--;
        if (m_rest == 0) {
          m_rest = m_decay_update_interval;
          if (m_level < ((T == 0) ? 0x0100 : 0x1000 /* gate for amp */)) {
            m_level = 0;
          } else {
            m_level = mul_q16_q8(m_level, ENV_GEN_DECAY_FACTOR);
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
template <uint8_t T> boolean  EnvGen<T>::m_sustain;
template <uint8_t T> uint8_t  EnvGen<T>::m_decay_update_interval;
template <uint8_t T> uint8_t  EnvGen<T>::m_rest;
