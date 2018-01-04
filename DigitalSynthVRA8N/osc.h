#pragma once

#include "common.h"
#include "osc-table.h"
#include "mul-q.h"
#include <math.h>

static const uint8_t OSC_MIX_TABLE_LENGTH = 32 + 1;  // odd number

template <uint8_t T>
class Osc {
  static uint8_t        m_count;
  static int8_t         m_mix_main;
  static int8_t         m_mix_detune;
  static int8_t         m_mix_sub;
  static int8_t         m_mix_table[OSC_MIX_TABLE_LENGTH];
  static uint8_t        m_detune;
  static uint8_t        m_detune_noise_gen_amt;
  static uint8_t        m_detune_mod_amt;
  static uint16_t       m_portamento;
  static uint8_t        m_rnd_prev;
  static uint8_t        m_waveform;
  static int16_t        m_pitch_bend_normalized;
  static uint16_t       m_pitch_target_array[3];
  static uint16_t       m_pitch_current_array[3];
  static const uint8_t* m_wave_table[3];
  static __uint24       m_freq_array[3];
  static uint16_t       m_freq_detune;
  static __uint24       m_phase_array[3];
  static __uint24       m_phase_detune;

public:
  INLINE static void initialize() {
    m_count = 0;
    for (uint8_t i = 0; i < OSC_MIX_TABLE_LENGTH; i++) {
      m_mix_table[i] = static_cast<uint8_t>(sqrtf(static_cast<float>(i) /
                                                  (OSC_MIX_TABLE_LENGTH - 1)) * 127);
    }
    m_mix_main   = m_mix_table[(OSC_MIX_TABLE_LENGTH - 1) >> 1];
    m_mix_detune = m_mix_table[(OSC_MIX_TABLE_LENGTH - 1) >> 1];
    m_mix_sub = 0;
    m_detune = 0;
    m_detune_noise_gen_amt = 0;
    m_detune_mod_amt = 0;
    m_portamento = 0x4000;
    m_rnd_prev = 0;
    m_waveform = OSC_WAVEFORM_SAW;
    m_pitch_bend_normalized = 0;
    m_pitch_target_array[0] = (NOTE_NUMBER_MIN + 0) << 8;
    m_pitch_target_array[1] = (NOTE_NUMBER_MIN + 2) << 8;
    m_pitch_target_array[2] = (NOTE_NUMBER_MIN + 4) << 8;
    m_pitch_current_array[0] = m_pitch_target_array[0];
    m_pitch_current_array[1] = m_pitch_target_array[1];
    m_pitch_current_array[2] = m_pitch_target_array[2];
    m_wave_table[0] = g_osc_saw_wave_tables[0];
    m_wave_table[1] = g_osc_saw_wave_tables[2];
    m_wave_table[2] = g_osc_saw_wave_tables[4];
    m_freq_array[0] = g_osc_freq_table[0];
    m_freq_array[1] = g_osc_freq_table[2];
    m_freq_array[2] = g_osc_freq_table[4];
    m_freq_detune = 0;
    m_phase_array[0] = 0;
    m_phase_array[1] = 0;
    m_phase_array[2] = 0;
    m_phase_detune = 0;
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

  INLINE static void note_on(uint8_t osc_number, uint8_t note_number) {
    m_pitch_target_array[0] = note_number << 8;
  }

  INLINE static void set_pitch_bend(int16_t pitch_bend) {
    pitch_bend++;
#if (PITCH_BEND_RANGE == 12)
    m_pitch_bend_normalized = ((pitch_bend << 1) + pitch_bend) >> 3;
#else  // (PITCH_BEND_RANGE == 2)
    m_pitch_bend_normalized = pitch_bend >> 4;
#endif
  }

  INLINE static int16_t clock(uint8_t amp_0, uint8_t amp_1, uint8_t amp_2, uint8_t mod_input) {
    m_count++;
    if ((m_count & (OSC_CONTROL_INTERVAL - 1)) == 0) {
      uint8_t idx = (m_count >> OSC_CONTROL_INTERVAL_BITS) & 0x07;
      switch (idx) {
      case 0:
        update_freq<0>();
        break;
      case 3:
        update_freq_detune(mod_input);
        break;
      case 4:
        update_pitch_current_array<0>();
        break;
      }
    }

    m_phase_array[0] += m_freq_array[0];
    m_phase_detune += m_freq_detune;

    int8_t wave_0_main,   wave_1_main,   wave_2_main;
    int8_t wave_0_detune, wave_1_detune, wave_2_detune;
    int16_t level_main;
    int16_t level_detune;
    int16_t result;
    {
      wave_0_main   = get_wave_level(m_wave_table[0],  m_phase_array[0]                   << 1);
      wave_0_detune = get_wave_level(m_wave_table[0], (m_phase_array[0] + m_phase_detune) << 1);

      // amp and mix
      {
        amp_0 = amp_0 + (amp_0 >> 1);
        level_main   = mul_q15_q7((wave_0_main   * amp_0), m_mix_main);
        level_detune = mul_q15_q7((wave_0_detune * amp_0), m_mix_detune);
      }
      result = level_main + level_detune;
    }

    return result;
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

  template <uint8_t N>
  INLINE static void update_freq() {
#if defined(TRANSPOSE)
    int16_t transpose = (TRANSPOSE << 8) | 0x00;
    uint16_t pitch_real = m_pitch_current_array[N] + m_pitch_bend_normalized + transpose;
#else
    uint16_t pitch_real = m_pitch_current_array[N] + m_pitch_bend_normalized;
#endif
    uint8_t pitch = high_byte(pitch_real);
    uint8_t pitch_fine = low_byte(pitch_real);
    if (pitch < NOTE_NUMBER_MIN) {
      pitch = NOTE_NUMBER_MIN;
    } else if (pitch > NOTE_NUMBER_MAX) {
      pitch = NOTE_NUMBER_MAX;
      pitch_fine = 0x00;
    }

    m_wave_table[N] = get_wave_table(m_waveform, pitch);
    __uint24 freq_base = g_osc_freq_table[pitch - NOTE_NUMBER_MIN];
    uint16_t freq_div_256 = freq_base >> 8;
    uint16_t freq_offset = freq_div_256 * g_osc_tune_table[pitch_fine >> (8 - OSC_TUNE_TABLE_STEPS_BITS)];
    m_freq_array[N] = freq_base + freq_offset;
  }

  template <uint8_t N>
  INLINE static void update_pitch_current_array() {
    if (m_pitch_current_array[N] + m_portamento < m_pitch_target_array[N]) {
      m_pitch_current_array[N] += m_portamento;
    } else if (m_pitch_current_array[N] > m_pitch_target_array[N] + m_portamento) {
      m_pitch_current_array[N] -= m_portamento;
    } else {
      m_pitch_current_array[N] = m_pitch_target_array[N];
    }
  }

  INLINE static void update_freq_detune(uint8_t mod_input) {
    int16_t detune_candidate = m_detune +
                               high_sbyte(((m_detune_mod_amt - 64) << 1) * mod_input);
    // TODO: Not to use IFilter
    uint8_t rnd = IFilter<0>::get_rnd8() >> 1;
    uint8_t red_noise = m_rnd_prev + rnd;
    detune_candidate += high_byte(m_detune_noise_gen_amt * red_noise);
    m_rnd_prev = rnd;
    uint8_t detune_target;
    if (detune_candidate > 127) {
      detune_target = 127;
    } else if (detune_candidate < 0) {
      detune_target = 0;
    } else {
      detune_target = detune_candidate;
    }

    m_freq_detune = (static_cast<uint16_t>(high_byte((detune_target << 1) *
                                           (detune_target << 1))) <<
                     OSC_DETUNE_MUL_NUM_BITS) + OSC_DETUNE_FREQ_MIN;
  }
};

template <uint8_t T> uint8_t         Osc<T>::m_count;
template <uint8_t T> int8_t          Osc<T>::m_mix_main;
template <uint8_t T> int8_t          Osc<T>::m_mix_detune;
template <uint8_t T> int8_t          Osc<T>::m_mix_sub;
template <uint8_t T> int8_t          Osc<T>::m_mix_table[OSC_MIX_TABLE_LENGTH];
template <uint8_t T> uint8_t         Osc<T>::m_detune;
template <uint8_t T> uint8_t         Osc<T>::m_detune_noise_gen_amt;
template <uint8_t T> uint8_t         Osc<T>::m_detune_mod_amt;
template <uint8_t T> uint16_t        Osc<T>::m_portamento;
template <uint8_t T> uint8_t         Osc<T>::m_rnd_prev;
template <uint8_t T> uint8_t         Osc<T>::m_waveform;
template <uint8_t T> int16_t         Osc<T>::m_pitch_bend_normalized;
template <uint8_t T> uint16_t        Osc<T>::m_pitch_target_array[3];
template <uint8_t T> uint16_t        Osc<T>::m_pitch_current_array[3];
template <uint8_t T> const uint8_t*  Osc<T>::m_wave_table[3];
template <uint8_t T> __uint24        Osc<T>::m_freq_array[3];
template <uint8_t T> uint16_t        Osc<T>::m_freq_detune;
template <uint8_t T> __uint24        Osc<T>::m_phase_array[3];
template <uint8_t T> __uint24        Osc<T>::m_phase_detune;
