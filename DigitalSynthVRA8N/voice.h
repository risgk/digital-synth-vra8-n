#include "common.h"
#include "program_table.h"

template <uint8_t T>
class Voice {
  static uint8_t m_count;
  static uint8_t m_eg0_decay_sustain;
  static uint8_t m_eg1_decay_sustain;
  static uint8_t m_current_note_number;
  static uint8_t m_on_note[16];
  static uint8_t m_output_error;
  static uint8_t m_portamento;
  static boolean m_legato;
  static boolean m_key_assign_last;
  static int8_t m_cutoff_velocity_amt;
  static uint8_t m_attack;
  static uint8_t m_decay;
  static uint8_t m_sustain;
  static uint8_t m_amp_env_gen;

public:
  INLINE static void initialize() {
    m_current_note_number = NOTE_NUMBER_INVALID;
    for (uint8_t i = 0; i < 16; ++i) {
      m_on_note[i] = 0x00;
    }
    m_output_error = 0;
    m_portamento = 0;
    m_legato = false;
    m_key_assign_last = false;
    IOsc<0>::initialize();
    IFilter<0>::initialize();
    IAmp<0>::initialize();
    IEnvGen<0>::initialize();
    IEnvGen<1>::initialize();
    m_attack = 0;
    m_decay = 0;
    m_sustain = 127;
    m_amp_env_gen = 127;
    update_env_gen();
  }

  INLINE static void note_on(uint8_t note_number, uint8_t velocity) {
    uint8_t cutoff_v = 64;
    if (m_cutoff_velocity_amt >= 0) {
      cutoff_v = high_sbyte((static_cast<int8_t>(velocity - 64) << 1) *
                            m_cutoff_velocity_amt) + 64;
    } else {
      cutoff_v = high_sbyte((static_cast<int8_t>(IOsc<0>::get_white_noise_7() - 64) << 1) *
                            m_cutoff_velocity_amt) + 64;
    }

    if (m_legato) {
      if (m_current_note_number != NOTE_NUMBER_INVALID) {
        IOsc<0>::set_portamento(m_portamento);
      } else {
        IOsc<0>::set_portamento(0);
        IFilter<0>::note_on(cutoff_v);
        IEnvGen<0>::note_on();
        IEnvGen<1>::note_on();
      }
    } else {
      IOsc<0>::set_portamento(m_portamento);
      if (m_key_assign_last || (m_current_note_number == NOTE_NUMBER_INVALID)) {
        IFilter<0>::note_on(cutoff_v);
        IEnvGen<0>::note_on();
        IEnvGen<1>::note_on();
      }
    }

    set_on_note(note_number);
    if (m_key_assign_last) {
      m_current_note_number = note_number;
    } else {
      m_current_note_number = get_active_on_note();
    }
    IOsc<0>::note_on<0>(m_current_note_number);
    IOsc<0>::note_on<1>(m_current_note_number);
  }

  INLINE static void note_off(uint8_t note_number) {
    clear_on_note(note_number);
    if (m_key_assign_last) {
      if (m_current_note_number == note_number) {
        all_note_off();
      }
    } else {
      uint8_t active_on_note = get_active_on_note();
      if (active_on_note == NOTE_NUMBER_INVALID) {
        all_note_off();
      } else {
        if (m_legato) {
          if (m_current_note_number != NOTE_NUMBER_INVALID) {
            IOsc<0>::set_portamento(m_portamento);
          } else {
            IOsc<0>::set_portamento(0);
            IEnvGen<0>::note_on();
            IEnvGen<1>::note_on();
          }
        } else {
          IOsc<0>::set_portamento(m_portamento);
          if (m_key_assign_last || (m_current_note_number == NOTE_NUMBER_INVALID)) {
            IEnvGen<0>::note_on();
            IEnvGen<1>::note_on();
          }
        }

        m_current_note_number = active_on_note;
        IOsc<0>::note_on<0>(m_current_note_number);
        IOsc<0>::note_on<1>(m_current_note_number);
      }
    }
  }

  INLINE static void all_note_off() {
    for (uint8_t i = 0; i < 16; ++i) {
      m_on_note[i] = 0x00;
    }
    m_current_note_number = NOTE_NUMBER_INVALID;
    IOsc<0>::note_off<0>();
    IOsc<0>::note_off<1>();
    IEnvGen<0>::note_off();
    IEnvGen<1>::note_off();
  }

  INLINE static void control_change(uint8_t controller_number, uint8_t controller_value) {
    switch (controller_number) {
    case MODULATION:
      IOsc<0>::set_lfo_depth<1>(controller_value);
      break;

    case FILTER_CUTOFF:
      IFilter<0>::set_cutoff(controller_value);
      break;
    case FILTER_RESO:
      IFilter<0>::set_resonance(controller_value);
      break;
    case CUTOFF_EG_AMT:
      IFilter<0>::set_cutoff_env_amt(controller_value);
      break;
    case EG_DECAY:
      m_decay = controller_value;
      if (m_amp_env_gen >= 64) {
        IEnvGen<0>::set_decay(m_decay);
        IEnvGen<1>::set_decay(m_decay);
      } else {
        IEnvGen<0>::set_decay(m_decay);
      }
      break;

    case OSC2_COARSE:
      IOsc<0>::set_pitch_offset_1(controller_value);
      break;
    case OSC2_FINE:
      IOsc<0>::set_detune(controller_value);
      break;
    case PORTAMENTO:
      m_portamento = controller_value;
      break;
    case EG_ATTACK:
      m_attack = controller_value;
      if (m_amp_env_gen >= 64) {
        IEnvGen<0>::set_attack(m_attack);
        IEnvGen<1>::set_attack(m_attack);
      } else {
        IEnvGen<0>::set_attack(m_attack);
      }
      break;

    case OSC_WAVE :
      IOsc<0>::set_waveform<0>(controller_value);
      IOsc<0>::set_waveform<1>(controller_value);
      break;
    case OSC2_MIX:
      IOsc<0>::set_osc_mix((controller_value + 1) >> 1);
      break;
    case SUB_OSC_MIX:
      IOsc<0>::set_sub_osc_level(controller_value);
      break;
    case EG_SUSTAIN:
      {
        m_sustain = controller_value;

        if (m_amp_env_gen >= 64) {
          IEnvGen<0>::set_sustain(m_sustain);
          IEnvGen<1>::set_sustain(m_sustain);
        } else {
          IEnvGen<0>::set_sustain(m_sustain);
        }
      }
      break;

    case SUB_OSC_WAVE:
      IOsc<0>::set_sub_waveform(controller_value);
      break;
    case LEGATO:
      if (controller_value < 64) {
        if (m_legato) {
          m_legato = false;
        }
      } else {
        if (!m_legato) {
          m_legato = true;
        }
      }
      break;
    case AMP_EG_ON:
      m_amp_env_gen = controller_value;
      update_env_gen();
      break;

    case LFO_RATE:
      IOsc<0>::set_lfo_rate(controller_value);
      break;
    case LFO_DEPTH:
      IOsc<0>::set_lfo_depth<0>(controller_value);
      break;
    case PITCH_LFO_AMT:
      IOsc<0>::set_pitch_lfo_amt<0>(controller_value);
      break;
    case CO_LFO_AMT:
      IFilter<0>::set_cutoff_lfo_amt(controller_value);
      break;

    case PB_RANGE:
      IOsc<0>::set_pitch_bend_minus_range(controller_value);
      IOsc<0>::set_pitch_bend_plus_range(controller_value);
      break;
    case KEY_ASSIGN:
      if (controller_value < 64) {
        if (m_key_assign_last) {
          m_key_assign_last = false;
        }
      } else {
        if (!m_key_assign_last) {
          m_key_assign_last = true;
        }
      }
      break;

    case ALL_NOTES_OFF:
    case OMNI_MODE_OFF:
    case OMNI_MODE_ON:
    case MONO_MODE_ON:
    case POLY_MODE_ON:
      all_note_off();
      break;
    }
  }

  INLINE static void pitch_bend(uint8_t lsb, uint8_t msb) {
    uint16_t pitch_bend = (msb << 7) + lsb - 8192;
    IOsc<0>::set_pitch_bend(pitch_bend);
  }

  static void program_change(uint8_t program_number) {
    if (program_number > 7) {
      return;
    }

    control_change(FILTER_CUTOFF, preset_table_FILTER_CUTOFF[program_number]);
    control_change(FILTER_RESO  , preset_table_FILTER_RESO  [program_number]);
    control_change(CUTOFF_EG_AMT, preset_table_CUTOFF_EG_AMT[program_number]);
    control_change(EG_DECAY     , preset_table_EG_DECAY     [program_number]);

    control_change(OSC2_COARSE  , preset_table_OSC2_COARSE  [program_number]);
    control_change(OSC2_FINE    , preset_table_OSC2_FINE    [program_number]);
    control_change(PORTAMENTO   , preset_table_PORTAMENTO   [program_number]);
    control_change(EG_ATTACK    , preset_table_EG_ATTACK    [program_number]);

    control_change(OSC_WAVE     , preset_table_OSC_WAVE     [program_number]);
    control_change(OSC2_MIX     , preset_table_OSC2_MIX     [program_number]);
    control_change(SUB_OSC_MIX  , preset_table_SUB_OSC_MIX  [program_number]);
    control_change(EG_SUSTAIN   , preset_table_EG_SUSTAIN   [program_number]);

    control_change(CC28         , preset_table_CC28         [program_number]);
    control_change(SUB_OSC_WAVE , preset_table_SUB_OSC_WAVE [program_number]);
    control_change(LEGATO       , preset_table_LEGATO       [program_number]);
    control_change(AMP_EG_ON    , preset_table_AMP_EG_ON    [program_number]);

    control_change(LFO_RATE     , preset_table_LFO_RATE     [program_number]);
    control_change(LFO_DEPTH    , preset_table_LFO_DEPTH    [program_number]);
    control_change(PITCH_LFO_AMT, preset_table_PITCH_LFO_AMT[program_number]);
    control_change(CO_LFO_AMT   , preset_table_CO_LFO_AMT   [program_number]);

    control_change(PB_RANGE     , preset_table_PB_RANGE     [program_number]);
    control_change(CC86         , preset_table_CC86         [program_number]);
    control_change(KEY_ASSIGN   , preset_table_KEY_ASSIGN   [program_number]);
    control_change(CC89         , preset_table_CC89         [program_number]);
  }

  INLINE static int8_t clock() {
    m_count++;

    int16_t osc_output = IOsc<0>::clock(m_count);
    int16_t lfo_output = IOsc<0>::get_lfo_level();
    uint8_t env_gen_output_0 = IEnvGen<0>::clock(m_count);
    int16_t filter_output = IFilter<0>::clock(m_count, osc_output, env_gen_output_0, lfo_output);
    uint8_t env_gen_output_1 = IEnvGen<1>::clock(m_count);
    int16_t amp_output = IAmp<0>::clock(filter_output, env_gen_output_1);

    // error diffusion
    int16_t output = amp_output + m_output_error;
    m_output_error = low_byte(output);
    return high_sbyte(output);
  }

private:
  INLINE static void set_on_note(uint8_t note_number) {
    m_on_note[note_number >> 3] |= (1 << (note_number & 0x07));
  }

  INLINE static void clear_on_note(uint8_t note_number) {
    m_on_note[note_number >> 3] &= ~(1 << (note_number & 0x07));
  }

  INLINE static uint8_t get_active_on_note() {
    return get_lowest_on_note();
  }

  INLINE static uint8_t get_highest_on_note() {
    uint8_t highest_on_note = NOTE_NUMBER_INVALID;
    for (int8_t i = 15; i >= 0; --i) {
      if (m_on_note[i] != 0x00) {
        if        (m_on_note[i] & 0x80) {
          highest_on_note = (i << 3) + 7;
        } else if (m_on_note[i] & 0x40) {
          highest_on_note = (i << 3) + 6;
        } else if (m_on_note[i] & 0x20) {
          highest_on_note = (i << 3) + 5;
        } else if (m_on_note[i] & 0x10) {
          highest_on_note = (i << 3) + 4;
        } else if (m_on_note[i] & 0x08) {
          highest_on_note = (i << 3) + 3;
        } else if (m_on_note[i] & 0x04) {
          highest_on_note = (i << 3) + 2;
        } else if (m_on_note[i] & 0x02) {
          highest_on_note = (i << 3) + 1;
        } else if (m_on_note[i] & 0x01) {
          highest_on_note = (i << 3) + 0;
        }
        break;
      }
    }
    return highest_on_note;
  }

  INLINE static uint8_t get_lowest_on_note() {
    uint8_t lowest_on_note = NOTE_NUMBER_INVALID;
    for (uint8_t i = 0; i < 16; ++i) {
      if (m_on_note[i] != 0x00) {
        if        (m_on_note[i] & 0x01) {
          lowest_on_note = (i << 3) + 0;
        } else if (m_on_note[i] & 0x02) {
          lowest_on_note = (i << 3) + 1;
        } else if (m_on_note[i] & 0x04) {
          lowest_on_note = (i << 3) + 2;
        } else if (m_on_note[i] & 0x08) {
          lowest_on_note = (i << 3) + 3;
        } else if (m_on_note[i] & 0x10) {
          lowest_on_note = (i << 3) + 4;
        } else if (m_on_note[i] & 0x20) {
          lowest_on_note = (i << 3) + 5;
        } else if (m_on_note[i] & 0x40) {
          lowest_on_note = (i << 3) + 6;
        } else if (m_on_note[i] & 0x80) {
          lowest_on_note = (i << 3) + 7;
        }
        break;
      }
    }
    return lowest_on_note;
  }

  INLINE static void update_env_gen() {
    if (m_amp_env_gen >= 64) {
      IEnvGen<0>::set_attack(m_attack);
      IEnvGen<0>::set_decay(m_decay);
      IEnvGen<0>::set_sustain(m_sustain);
      IEnvGen<1>::set_attack(m_attack);
      IEnvGen<1>::set_decay(m_decay);
      IEnvGen<1>::set_sustain(m_sustain);
    } else {
      IEnvGen<0>::set_attack(m_attack);
      IEnvGen<0>::set_decay(m_decay);
      IEnvGen<0>::set_sustain(m_sustain);
      IEnvGen<1>::set_attack(0);
      IEnvGen<1>::set_decay(m_amp_env_gen << 1);
      IEnvGen<1>::set_sustain(127);
    }
  }
};

template <uint8_t T> uint8_t Voice<T>::m_count;
template <uint8_t T> uint8_t Voice<T>::m_eg0_decay_sustain;
template <uint8_t T> uint8_t Voice<T>::m_eg1_decay_sustain;
template <uint8_t T> uint8_t Voice<T>::m_current_note_number;
template <uint8_t T> uint8_t Voice<T>::m_on_note[16];
template <uint8_t T> uint8_t Voice<T>::m_output_error;
template <uint8_t T> uint8_t Voice<T>::m_portamento;
template <uint8_t T> boolean Voice<T>::m_legato;
template <uint8_t T> boolean Voice<T>::m_key_assign_last;
template <uint8_t T> int8_t Voice<T>::m_cutoff_velocity_amt;
template <uint8_t T> uint8_t Voice<T>::m_attack;
template <uint8_t T> uint8_t Voice<T>::m_decay;
template <uint8_t T> uint8_t Voice<T>::m_sustain;
template <uint8_t T> uint8_t Voice<T>::m_amp_env_gen;
