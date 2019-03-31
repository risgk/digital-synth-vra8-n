#pragma once

#include "common.h"

#define USE_INPUT_A0
#define USE_INPUT_A1
#define USE_INPUT_A2
#define USE_INPUT_A3

#define USE_INPUT_D2
#define USE_INPUT_D4
//#define DIGITAL_INPUT_ACTIVE_LOW  // For MIDI Shield

template <uint8_t T>
class CVIn {
  static const uint8_t CV_IN_CONTROL_INTERVAL_BITS = 1;
  static const uint8_t CV_IN_CONTROL_INTERVAL      = 0x01 << CV_IN_CONTROL_INTERVAL_BITS;

  static uint8_t m_count;
  static uint8_t m_note_number;

public:
  INLINE static void initialize() {
#if defined(EXPERIMENTAL_ENABLE_VOLTAGE_CONTROL)
    m_count = 0;
    m_note_number = NOTE_NUMBER_INVALID;

  #if defined(USE_INPUT_D2)
    pinMode(2, INPUT);
  #endif
  #if defined(USE_INPUT_D4)
    pinMode(4, INPUT);
  #endif
#endif
  }

  INLINE static void clock() {
#if defined(EXPERIMENTAL_ENABLE_VOLTAGE_CONTROL)
    ++m_count;

    if ((m_count & (CV_IN_CONTROL_INTERVAL - 1)) == 1) {
      uint8_t idx = (m_count >> CV_IN_CONTROL_INTERVAL_BITS) & 0x1F;

      uint16_t value;
      switch (idx) {
      case 0x0:
  #if defined(USE_INPUT_A0)
        adc_start<0>();
  #endif
        break;
      case 0x4:
  #if defined(USE_INPUT_A0)
        value = adc_read();    // Read A0
        value = (value + 1) * 15;
        set_note_number(high_byte(value) + 24);
  #endif
  #if defined(USE_INPUT_A1)
        adc_start<1>();
  #endif
        break;
      case 0x8:
  #if defined(USE_INPUT_A1)
        value = adc_read();    // Read A1
        IVoice<0>::control_change(OSC_MIX, value >> 3);
  #endif
  #if defined(USE_INPUT_A2)
        adc_start<2>();
  #endif
        break;
      case 0xC:
  #if defined(USE_INPUT_A2)
        value = adc_read();    // Read A2
        IVoice<0>::control_change(FILTER_CUTOFF, value >> 3);
  #endif
  #if defined(USE_INPUT_A3)
        adc_start<3>();
  #endif
        break;
      case 0x10:
  #if defined(USE_INPUT_A3)
        value = adc_read();    // Read A3
        IVoice<0>::control_change(FILTER_RESO, value >> 3);
  #endif
        break;
      case 0x14:
  #if defined(USE_INPUT_D2)
        value = digitalRead(2);    // Read D2
        if (value == HIGH) {
          IVoice<0>::program_change(PROGRAM_NUMBER_RANDOM);
        }
  #endif
        break;
      case 0x18:
  #if defined(USE_INPUT_D4)
        value = digitalRead(4);    // Read D4
  #endif
        break;
      case 0x1C:
        break;
      }
    }
#else // !defined(EXPERIMENTAL_ENABLE_VOLTAGE_CONTROL)
    static_cast<void>(count);
#endif
  }

private:
  // Start the conversion.
  template <uint8_t N>
  INLINE static void adc_start() {
    ADMUX = _BV(REFS0) | N;  // analogReference(DEFAULT)
    ADCSRA = _BV(ADEN) | _BV(ADSC) | 0b111;
  }

  // CAUTION: Call after the conversion is finishd.
  INLINE static uint16_t adc_read() {
    uint8_t adcLow  = ADCL;
    uint8_t adcHigh = ADCH;
    return ((adcHigh << 8) | adcLow);
  }

  INLINE static void set_note_number(uint8_t note_number) {
    // TODO
    uint8_t n = note_number;
    if (n == 24) {
      n = NOTE_NUMBER_INVALID;
    }

    if (m_note_number != n) {
      if (n != NOTE_NUMBER_INVALID) {
        IVoice<0>::note_on(n, 127);
      }
      IVoice<0>::note_off(m_note_number);
      m_note_number = n;
    }
  }
};

template <uint8_t T> uint8_t CVIn<T>::m_count;
template <uint8_t T> uint8_t CVIn<T>::m_note_number;
