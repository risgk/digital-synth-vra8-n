#pragma once

#include "common.h"
#include "mul-q.h"

template <uint8_t T>
class EnvGen {
  static const uint8_t STATE_ATTACK  = 0;
  static const uint8_t STATE_SUSTAIN = 1;
  static const uint8_t STATE_IDLE    = 2;

  static const uint8_t ATTACK_UPDATE_INTERVAL  = 1;

  static uint8_t  m_state;
  static uint16_t m_level;
  static uint16_t m_attack_step;
  static uint8_t  m_decay_update_interval;
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
    m_attack_step = (high_byte((127 - controller_value) *
                               (127 - controller_value)) + 1) << 8;
  }

  INLINE static void set_decay(uint8_t controller_value) {
    if (controller_value >= 128) {
      // No Decay
      m_decay_update_interval = 0;
    } else if (controller_value >= 32) {
      m_decay_update_interval = high_byte((controller_value << 1) *
                                          (controller_value << 1)) + 1;
    } else {
      m_decay_update_interval = (controller_value >> 1) + 1;
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
          if (m_level >= ENV_GEN_LEVEL_MAX - m_attack_step) {
            m_level = ENV_GEN_LEVEL_MAX;
            m_state = STATE_SUSTAIN;
            m_rest = m_decay_update_interval;
          } else {
            m_level += m_attack_step;
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
        if (m_decay_update_interval == 0) {
          break;
        }
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
template <uint8_t T> uint16_t EnvGen<T>::m_attack_step;
template <uint8_t T> uint8_t  EnvGen<T>::m_decay_update_interval;
template <uint8_t T> boolean  EnvGen<T>::m_sustain;
template <uint8_t T> uint8_t  EnvGen<T>::m_rest;
