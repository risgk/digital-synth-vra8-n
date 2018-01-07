#pragma once

#include "common.h"
#include "mul-q.h"

template <uint8_t T>
class Gate {
  static uint8_t m_target;
  static uint8_t m_current;
  static uint8_t m_count;

public:
  INLINE static void initialize() {
    m_target = 0;
    m_current = 0;
    m_count = 1;
  }

  INLINE static void note_on() {
    m_target = 63;
  }

  INLINE static void note_off() {
    m_target = 0;
  }

  INLINE static uint8_t clock() {
    m_count++;
    if ((m_count & (GATE_CONTROL_INTERVAL - 1)) == 0) {
      update_level();
    }
    return m_current;
  }

private:
  INLINE static void update_level() {
    if (m_current + GATE_LEVEL_STEP < m_target) {
      m_current += GATE_LEVEL_STEP;
    } else if (m_current > m_target + GATE_LEVEL_STEP) {
      m_current -= GATE_LEVEL_STEP;
    } else {
      m_current = m_target;
    }
  }
};

template <uint8_t T> uint8_t Gate<T>::m_target;
template <uint8_t T> uint8_t Gate<T>::m_current;
template <uint8_t T> uint8_t Gate<T>::m_count;
