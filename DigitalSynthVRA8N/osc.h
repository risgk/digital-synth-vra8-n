#pragma once

#include "common.h"
#include "osc-table.h"
#include "mul-q.h"
#include <math.h>

static const uint8_t OSC_MIX_TABLE_LENGTH = 31;

template <uint8_t T>
class Osc {
  static int8_t         m_mix_main;
  static int8_t         m_mix_detune;
  static int8_t         m_mix_sub;
  static int16_t        m_level_sub;
  static int8_t         m_mix_table[OSC_MIX_TABLE_LENGTH];
  static uint16_t       m_detune;
  static uint8_t        m_fluctuation;
  static uint8_t        m_detune_mod_amt;
  static uint16_t       m_portamento;
  static int8_t         m_mod_level;
  static uint16_t       m_lfo_phase;
  static int8_t         m_lfo_level;
  static uint16_t       m_lfo_rate;
  static uint8_t        m_lfo_depth[2];
  static uint8_t        m_waveform;
  static int16_t        m_pitch_bend_normalized;
  static uint16_t       m_pitch_target;
  static uint16_t       m_pitch_current;
  static uint16_t       m_pitch_real[2];
  static const uint8_t* m_wave_table[2];
  static const uint8_t* m_wave_table_temp[2];
  static __uint24       m_freq[2];
  static __uint24       m_freq_temp[2];
  static __uint24       m_phase[2];
  static uint8_t        m_rnd_cnt;
  static uint16_t       m_rnd_temp;
  static uint8_t        m_rnd;
  static uint8_t        m_rnd_prev;

public:
  INLINE static void initialize() {
    for (uint8_t i = 0; i < OSC_MIX_TABLE_LENGTH; i++) {
      m_mix_table[i] = static_cast<uint8_t>(sqrtf(static_cast<float>(i) /
                                                  (OSC_MIX_TABLE_LENGTH - 1)) * 80);
    }
    set_mix(0);
    set_sub_osc_level(0);
    m_level_sub = 0;
    m_detune = 0;
    m_fluctuation = 0;
    m_detune_mod_amt = 0;
    m_portamento = 0x4000;
    m_mod_level = 0;
    m_lfo_phase = 0;
    m_lfo_level = 0;
    m_lfo_rate = 0;
    m_lfo_depth[0] = 0;
    m_lfo_depth[1] = 0;
    m_waveform = OSC_WAVEFORM_SAW;
    m_pitch_bend_normalized = 0;
    m_pitch_target = (NOTE_NUMBER_MIN - (12 + 2)) << 8;
    m_pitch_current = m_pitch_target;
    m_pitch_real[0] = m_pitch_target;
    m_pitch_real[1] = m_pitch_target;
    m_wave_table[0] = g_osc_saw_wave_tables[0];
    m_wave_table[1] = g_osc_saw_wave_tables[0];
    m_wave_table_temp[0] = g_osc_saw_wave_tables[0];
    m_wave_table_temp[1] = g_osc_saw_wave_tables[0];
    m_freq[0] = g_osc_freq_table[0];
    m_freq[1] = g_osc_freq_table[0];
    m_freq_temp[0] = g_osc_freq_table[0];
    m_freq_temp[1] = g_osc_freq_table[0];
    m_phase[0] = 0;
    m_phase[1] = 0;
    m_rnd_cnt = 0;
    m_rnd_temp = 1;
    m_rnd = 0;
    m_rnd_prev = 0;
  }

  INLINE static void set_mix(uint8_t controller_value) {
    if (controller_value >= 2) {
      controller_value -= 2;
    }

    if (controller_value > 123) {
      controller_value = 123;
    }

    m_mix_main   = m_mix_table[(OSC_MIX_TABLE_LENGTH - 1) - (controller_value >> 2)];
    m_mix_detune = m_mix_table[                             (controller_value >> 2)];
  }

  INLINE static void set_waveform(uint8_t controller_value) {
    if (controller_value < 64) {
      m_waveform = OSC_WAVEFORM_SAW;
    } else {
      m_waveform = OSC_WAVEFORM_SQ;
    }
  }

  INLINE static void set_sub_osc_level(uint8_t controller_value) {
    uint8_t idx = controller_value >> 2;
    if (idx > 0) {
      idx -= 1;
    }
    m_mix_sub = m_mix_table[idx];
  }

  INLINE static void set_detune(uint8_t controller_value) {
    if (controller_value >= 15) {
      m_detune = (controller_value - 11) << 4;
    } else if (controller_value >= 2) {
      m_detune = (controller_value << 2) + 4;
    } else {
      m_detune = 9;
    }
  }

  INLINE static void set_detune_noise_gen_amt(uint8_t controller_value) {
    m_fluctuation = controller_value >> 1;
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

  INLINE static void set_lfo_rate(uint8_t controller_value) {
    m_lfo_rate = (controller_value + 1) * 25;
  }

  template <uint8_t N>
  INLINE static void set_lfo_depth(uint8_t controller_value) {
    m_lfo_depth[N] = controller_value;
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
    if ((count & 0x01) == 1) {
      int16_t wave_0_sub = get_tri_wave_level(m_phase[0] >> 8);
      m_level_sub = wave_0_sub * m_mix_sub;
    }
    else if ((count & (OSC_CONTROL_INTERVAL - 1)) == 0) {
      uint8_t idx = (count >> OSC_CONTROL_INTERVAL_BITS) & 0x0F;
      switch (idx) {
      case 0x0:
        update_pitch_current<0>();
        break;
      case 0x1:
        update_freq_first<0>();
        break;
      case 0x2:
        update_freq_latter<0>();
        break;
      case 0x4:
        m_rnd_cnt++;
        if ((m_rnd_cnt & 0x03) == 0x00) {
          update_rnd();
        }
        break;
      case 0x8:
        update_pitch_current<1>();
        break;
      case 0x9:
        update_freq_first<1>();
        break;
      case 0xA:
        update_freq_latter<1>();
        break;
      case 0xC:
        if ((m_rnd_cnt & 0x03) == 0x00) {
          update_rnd();
        }
        break;
      case 0xE:
        update_lfo();
        break;
      }
    }

    m_phase[0] += m_freq[0];
    m_phase[1] += m_freq[1];

    int8_t wave_0_main   = get_wave_level(m_wave_table[0], static_cast<uint16_t>(m_phase[0] >> 8) << 1);
    int8_t wave_0_detune = get_wave_level(m_wave_table[1], static_cast<uint16_t>(m_phase[1] >> 8) << 1);

    // amp and mix
    int16_t level_main   = wave_0_main   * m_mix_main;
    int16_t level_detune = wave_0_detune * m_mix_detune;
    int16_t result       = level_main + level_detune + m_level_sub;

    return result;
  }

private:
  INLINE static const uint8_t* get_wave_table(uint8_t waveform, uint8_t note_number) {
    const uint8_t* result;
    if (waveform == OSC_WAVEFORM_SAW) {
      result = g_osc_saw_wave_tables[note_number - (NOTE_NUMBER_MIN - (12 + 2))];
    } else {
      result = g_osc_sq_wave_tables[note_number - (NOTE_NUMBER_MIN - (12 + 2))];
    }
    return result;
  }

  INLINE static int8_t get_wave_level(const uint8_t* wave_table, uint16_t phase) {
    uint8_t curr_index = high_byte(phase);
    uint8_t next_weight = low_byte(phase);
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

  INLINE static int8_t get_tri_wave_level(uint16_t phase) {
    uint16_t level = phase;
    if ((level & 0x8000) != 0) {
      level = ~level;
    }
    level -= 0x4000;
    return high_sbyte(level << 1);
  }

  template <uint8_t N>
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
    m_pitch_real[N] = m_pitch_current + m_pitch_bend_normalized + transpose;
#else
    m_pitch_real[N] = m_pitch_current + m_pitch_bend_normalized;
#endif

    uint8_t coarse = high_byte(m_pitch_real[N]);
    if (coarse <= (NOTE_NUMBER_MIN - 12)) {
      m_pitch_real[N] = (NOTE_NUMBER_MIN - 12) << 8;
    } else if (coarse >= (NOTE_NUMBER_MAX + 12)) {
      m_pitch_real[N] = (NOTE_NUMBER_MAX + 12) << 8;
    }

    if (N == 1) {
      /* For OSC 2 */
      m_pitch_real[N] += m_detune;
    }
    m_pitch_real[N] += m_mod_level;
    m_pitch_real[N] += m_mod_level;
  }

  template <uint8_t N>
  INLINE static void update_freq_first() {
    m_pitch_real[N] += high_sbyte((m_fluctuation >> 2) * static_cast<int8_t>(get_red_noise_8()));
    m_pitch_real[N] += 128;
    uint8_t coarse = high_byte(m_pitch_real[N]);
    m_freq_temp[N] = g_osc_freq_table[coarse - (NOTE_NUMBER_MIN - (12 + 2))];
    m_wave_table_temp[N] = get_wave_table(m_waveform, coarse);
  }

  template <uint8_t N>
  INLINE static void update_freq_latter() {
    uint8_t fine = low_byte(m_pitch_real[N]);
    uint16_t freq_div_512 = m_freq_temp[N] >> 8;
    freq_div_512 >>= 1;
    int16_t freq_offset = (freq_div_512 * g_osc_tune_table[fine >> (8 - OSC_TUNE_TABLE_STEPS_BITS)]) + (N << 1);
    m_freq[N] = m_freq_temp[N] + freq_offset;
    m_wave_table[N] = m_wave_table_temp[N];
  }

  INLINE static void update_rnd() {
    m_rnd_temp = m_rnd_temp ^ (m_rnd_temp << 5);
    m_rnd_temp = m_rnd_temp ^ (m_rnd_temp >> 9);
    m_rnd_temp = m_rnd_temp ^ (m_rnd_temp << 8);
    m_rnd_prev = m_rnd;
    m_rnd = low_byte(m_rnd_temp) >> 1;
  }

  INLINE static uint8_t get_red_noise_8() {
    return (m_rnd_prev + m_rnd);
  }

  INLINE static void update_lfo() {
    m_lfo_phase += m_lfo_rate;
    m_lfo_level = get_tri_wave_level(m_lfo_phase);
    uint8_t lfo_depth = m_lfo_depth[0] + m_lfo_depth[1];
    if (lfo_depth > 127) {
      lfo_depth = 127;
    }
    m_mod_level = high_sbyte((lfo_depth << 1) * m_lfo_level);
  }
};

template <uint8_t T> int8_t          Osc<T>::m_mix_main;
template <uint8_t T> int8_t          Osc<T>::m_mix_detune;
template <uint8_t T> int8_t          Osc<T>::m_mix_sub;
template <uint8_t T> int16_t         Osc<T>::m_level_sub;
template <uint8_t T> int8_t          Osc<T>::m_mix_table[OSC_MIX_TABLE_LENGTH];
template <uint8_t T> uint16_t        Osc<T>::m_detune;
template <uint8_t T> uint8_t         Osc<T>::m_fluctuation;
template <uint8_t T> uint8_t         Osc<T>::m_detune_mod_amt;
template <uint8_t T> uint16_t        Osc<T>::m_portamento;
template <uint8_t T> int8_t          Osc<T>::m_mod_level;
template <uint8_t T> uint16_t        Osc<T>::m_lfo_phase;
template <uint8_t T> int8_t          Osc<T>::m_lfo_level;
template <uint8_t T> uint16_t        Osc<T>::m_lfo_rate;
template <uint8_t T> uint8_t         Osc<T>::m_lfo_depth[2];
template <uint8_t T> uint8_t         Osc<T>::m_waveform;
template <uint8_t T> int16_t         Osc<T>::m_pitch_bend_normalized;
template <uint8_t T> uint16_t        Osc<T>::m_pitch_target;
template <uint8_t T> uint16_t        Osc<T>::m_pitch_current;
template <uint8_t T> uint16_t        Osc<T>::m_pitch_real[2];
template <uint8_t T> const uint8_t*  Osc<T>::m_wave_table[2];
template <uint8_t T> const uint8_t*  Osc<T>::m_wave_table_temp[2];
template <uint8_t T> __uint24        Osc<T>::m_freq[2];
template <uint8_t T> __uint24        Osc<T>::m_freq_temp[2];
template <uint8_t T> __uint24        Osc<T>::m_phase[2];
template <uint8_t T> uint8_t         Osc<T>::m_rnd_cnt;
template <uint8_t T> uint16_t        Osc<T>::m_rnd_temp;
template <uint8_t T> uint8_t         Osc<T>::m_rnd;
template <uint8_t T> uint8_t         Osc<T>::m_rnd_prev;
