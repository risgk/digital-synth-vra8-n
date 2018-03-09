#pragma once

#include "common.h"
#include "osc-table.h"
#include "mul-q.h"
#include <math.h>

static const uint8_t OSC_MIX_TABLE_LENGTH = 32 + 1;  // odd number

template <uint8_t T>
class Osc {
  static int8_t         m_mix_main;
  static int8_t         m_mix_detune;
  static int8_t         m_mix_sub;
  static int16_t        m_level_sub;
  static int8_t         m_mix_table[OSC_MIX_TABLE_LENGTH];
  static uint8_t        m_detune;
  static uint8_t        m_detune_noise_gen_amt;
  static uint8_t        m_detune_mod_amt;
  static uint16_t       m_portamento;
  static uint8_t        m_waveform;
  static int16_t        m_pitch_bend_normalized;
  static uint16_t       m_pitch_target;
  static uint16_t       m_pitch_current;
  static uint16_t       m_pitch_real;
  static const uint8_t* m_wave_table;
  static const uint8_t* m_wave_table_temp;
  static __uint24       m_freq;
  static __uint24       m_freq_temp;
  static uint16_t       m_freq_detune;
  static __uint24       m_phase;
  static __uint24       m_phase_detune;
  static uint16_t       m_rnd_temp;
  static uint8_t        m_rnd;
  static uint8_t        m_rnd_prev;

public:
  INLINE static void initialize() {
    for (uint8_t i = 0; i < OSC_MIX_TABLE_LENGTH; i++) {
      m_mix_table[i] = static_cast<uint8_t>(sqrtf(static_cast<float>(i) /
                                                  (OSC_MIX_TABLE_LENGTH - 1)) * 40);
    }
    m_mix_main   = m_mix_table[(OSC_MIX_TABLE_LENGTH - 1) >> 1];
    m_mix_detune = m_mix_table[(OSC_MIX_TABLE_LENGTH - 1) >> 1];
    m_mix_sub = 0;
    m_level_sub = 0;
    m_detune = 0;
    m_detune_noise_gen_amt = 0;
    m_detune_mod_amt = 0;
    m_portamento = 0x4000;
    m_waveform = OSC_WAVEFORM_SAW;
    m_pitch_bend_normalized = 0;
    m_pitch_target = (NOTE_NUMBER_MIN + 0) << 8;
    m_pitch_current = m_pitch_target;
    m_pitch_real = m_pitch_target;
    m_wave_table = g_osc_saw_wave_tables[0];
    m_wave_table_temp = g_osc_saw_wave_tables[0];
    m_freq = g_osc_freq_table[0];
    m_freq_temp = g_osc_freq_table[0];
    m_freq_detune = 0;
    m_phase = 0;
    m_phase_detune = 0;
    m_rnd_temp = 1;
    m_rnd = 0;
    m_rnd_prev = 0;
  }

  INLINE static void set_mix(uint8_t controller_value) {
    uint8_t c = controller_value >> 1;
    const uint8_t L = (OSC_MIX_TABLE_LENGTH - 1) >> 1;
    if (c < 16) {
      m_mix_main   = +m_mix_table[L + c];
      m_mix_detune = +m_mix_table[L - c];
    } else if (c < 32) {
      m_mix_main   = +m_mix_table[L + 16];
      m_mix_detune = +m_mix_table[L - 16];
    } else if (c < 48) {
      m_mix_main   = +m_mix_table[L + (48 - c)];
      m_mix_detune = -m_mix_table[L - (48 - c)];
    } else {
      m_mix_main   = +m_mix_table[L];
      m_mix_detune = -m_mix_table[L];
    }
  }

  INLINE static void set_waveform(uint8_t waveform) {
    m_waveform = waveform;
  }

  INLINE static void set_sub(uint8_t controller_value) {
    m_mix_sub = m_mix_table[controller_value >> 2];
  }

  INLINE static void set_detune(uint8_t controller_value) {
    m_detune = controller_value;
  }

  INLINE static void set_detune_noise_gen_amt(uint8_t controller_value) {
    m_detune_noise_gen_amt = controller_value;
  }

  INLINE static void set_detune_env_amt(uint8_t controller_value) {
    m_detune_mod_amt = controller_value;
  }

  INLINE static void set_portamento(uint8_t controller_value) {
    if (controller_value < 4) {
      m_portamento = 0x4000;
    } else {
      m_portamento = 134 - ((controller_value >> 1) << 1);
    }
  }

  INLINE static void note_on(uint8_t note_number) {
    m_pitch_target = note_number << 8;
  }

  INLINE static void set_pitch_bend(int16_t pitch_bend) {
    pitch_bend++;
#if (PITCH_BEND_RANGE == 12)
    m_pitch_bend_normalized = ((pitch_bend << 1) + pitch_bend) >> 3;
#else  // (PITCH_BEND_RANGE == 2)
    m_pitch_bend_normalized = pitch_bend >> 4;
#endif
  }

  INLINE static int16_t clock(uint8_t count) {
    if ((count & (OSC_CONTROL_INTERVAL - 1)) == 0) {
      uint8_t idx = (count >> OSC_CONTROL_INTERVAL_BITS) & 0x07;
      switch (idx) {
      case 0:
        update_pitch_current();
        break;
      case 1:
        update_freq_first();
        break;
      case 2:
        update_freq_latter();
        break;
      case 4:
//        update_freq_detune();
        break;
      case 5:
//        update_freq_detune();
        break;
      case 6:
        update_rnd_first();
        break;
      case 7:
        update_rnd_latter();
        break;
      }
    }

    m_phase += m_freq;
    m_phase_detune += m_freq_detune;

    int8_t wave_0_main   = get_wave_level(m_wave_table,  m_phase                   << 1);
    int8_t wave_0_detune = get_wave_level(m_wave_table, (m_phase + m_phase_detune) << 1);

    if ((count & 0x01) == 1) {
      int8_t wave_0_sub = get_tri_wave_level(m_phase);
      m_level_sub       = wave_0_sub * m_mix_sub;
    }

    // amp and mix
    int16_t level_main   = wave_0_main   * m_mix_main;
    int16_t level_detune = wave_0_detune * m_mix_detune;
    int16_t result       = level_main + level_detune + m_level_sub;

    return result;
  }

  INLINE static uint8_t get_rnd8() {
    return m_rnd;
  }

private:
  INLINE static const uint8_t* get_wave_table(uint8_t waveform, uint8_t note_number) {
    const uint8_t* result;
    if (waveform == OSC_WAVEFORM_SAW) {
      result = g_osc_saw_wave_tables[note_number - NOTE_NUMBER_MIN];
    } else {
      result = g_osc_sq_wave_tables[note_number - NOTE_NUMBER_MIN];
    }
    return result;
  }

  INLINE static int8_t get_wave_level(const uint8_t* wave_table, __uint24 phase) {
    uint8_t curr_index = hhigh_byte(phase);
    uint8_t next_weight = high_byte(phase);
    uint16_t two_data = pgm_read_word(wave_table + curr_index);
    uint8_t curr_data = low_byte(two_data);
    uint8_t next_data = high_byte(two_data);

    // lerp
    int8_t result;
    if (static_cast<int8_t>(curr_data) < static_cast<int8_t>(next_data)) {
      result = curr_data + high_byte(static_cast<uint8_t>(next_data - curr_data) * next_weight);
    } else {
      result = curr_data - high_byte(static_cast<uint8_t>(curr_data - next_data) * next_weight);
    }

    return result;
  }

  INLINE static int8_t get_tri_wave_level(__uint24 phase) {
    uint16_t level = phase >> 8;
    if ((level & 0x8000) != 0) {
      level = ~level;
    }
    level -= 0x4000;
    return high_sbyte(level << 1);
  }

  INLINE static void update_pitch_current() {
    if (m_pitch_current + m_portamento < m_pitch_target) {
      m_pitch_current += m_portamento;
    } else if (m_pitch_current > m_pitch_target + m_portamento) {
      m_pitch_current -= m_portamento;
    } else {
      m_pitch_current = m_pitch_target;
    }

#if defined(TRANSPOSE)
    int16_t transpose = (TRANSPOSE << 8) | 0x00;
    m_pitch_real = m_pitch_current + m_pitch_bend_normalized + transpose;
#else
    m_pitch_real = m_pitch_current + m_pitch_bend_normalized;
#endif
    m_pitch_real += high_sbyte(m_detune_noise_gen_amt * static_cast<int8_t>(get_rnd8()));
  }

  INLINE static void update_freq_first() {
    uint8_t coarse = high_byte(m_pitch_real);
    if (coarse <= NOTE_NUMBER_MIN) {
      m_pitch_real = NOTE_NUMBER_MIN << 8;
    } else if (coarse >= NOTE_NUMBER_MAX) {
      m_pitch_real = NOTE_NUMBER_MAX << 8;
    }
    m_pitch_real += 128;

    coarse = high_byte(m_pitch_real);
    m_freq_temp = g_osc_freq_table[coarse - NOTE_NUMBER_MIN];
    m_wave_table_temp = get_wave_table(m_waveform, coarse);
  }

  INLINE static void update_freq_latter() {
    uint8_t fine = low_byte(m_pitch_real);
    uint16_t freq_div_16 = m_freq_temp >> 8;
    freq_div_16 <<= 4;
    int16_t freq_offset = mul_q16_q7(freq_div_16, g_osc_tune_table[fine >> (8 - OSC_TUNE_TABLE_STEPS_BITS)]);
    m_freq = m_freq_temp + freq_offset;
    m_wave_table = m_wave_table_temp;
  }

  INLINE static void update_freq_detune() {
    uint8_t rnd = get_rnd8() >> 1;
    uint8_t red_noise = m_rnd_prev + rnd;
    int16_t detune_candidate = m_detune + high_byte(m_detune_noise_gen_amt * red_noise);
    m_rnd_prev = rnd;
    uint8_t detune_target;
    if (detune_candidate > 127) {
      detune_target = 127;
    } else if (detune_candidate < 0) {
      detune_target = 0;
    } else {
      detune_target = detune_candidate;
    }

    uint16_t freq_div_512 = m_freq >> 8;
    freq_div_512 >>= 1;
    m_freq_detune = freq_div_512 * g_osc_tune_table[(detune_target + 9) >> (8 - OSC_TUNE_TABLE_STEPS_BITS)];
  }

  INLINE static void update_rnd_first() {
    m_rnd_temp = m_rnd_temp ^ (m_rnd_temp << 5);
    m_rnd_temp = m_rnd_temp ^ (m_rnd_temp >> 9);
  }

  INLINE static void update_rnd_latter() {
    m_rnd_temp = m_rnd_temp ^ (m_rnd_temp << 8);
    m_rnd = low_byte(m_rnd_temp);
  }
};

template <uint8_t T> int8_t          Osc<T>::m_mix_main;
template <uint8_t T> int8_t          Osc<T>::m_mix_detune;
template <uint8_t T> int8_t          Osc<T>::m_mix_sub;
template <uint8_t T> int16_t         Osc<T>::m_level_sub;
template <uint8_t T> int8_t          Osc<T>::m_mix_table[OSC_MIX_TABLE_LENGTH];
template <uint8_t T> uint8_t         Osc<T>::m_detune;
template <uint8_t T> uint8_t         Osc<T>::m_detune_noise_gen_amt;
template <uint8_t T> uint8_t         Osc<T>::m_detune_mod_amt;
template <uint8_t T> uint16_t        Osc<T>::m_portamento;
template <uint8_t T> uint8_t         Osc<T>::m_waveform;
template <uint8_t T> int16_t         Osc<T>::m_pitch_bend_normalized;
template <uint8_t T> uint16_t        Osc<T>::m_pitch_target;
template <uint8_t T> uint16_t        Osc<T>::m_pitch_current;
template <uint8_t T> uint16_t        Osc<T>::m_pitch_real;
template <uint8_t T> const uint8_t*  Osc<T>::m_wave_table;
template <uint8_t T> const uint8_t*  Osc<T>::m_wave_table_temp;
template <uint8_t T> __uint24        Osc<T>::m_freq;
template <uint8_t T> __uint24        Osc<T>::m_freq_temp;
template <uint8_t T> uint16_t        Osc<T>::m_freq_detune;
template <uint8_t T> __uint24        Osc<T>::m_phase;
template <uint8_t T> __uint24        Osc<T>::m_phase_detune;
template <uint8_t T> uint16_t        Osc<T>::m_rnd_temp;
template <uint8_t T> uint8_t         Osc<T>::m_rnd;
template <uint8_t T> uint8_t         Osc<T>::m_rnd_prev;
