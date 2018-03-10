#include "common.h"

template <uint8_t T>
class Voice {
  static uint8_t m_count;
  static uint8_t m_waveform;
  static uint8_t m_eg0_decay_sustain;
  static uint8_t m_eg1_decay_sustain;
  static boolean m_damper_pedal;
  static uint8_t m_note_number;
  static boolean m_note_hold;
  static uint8_t m_output_error;

public:
  INLINE static void initialize() {
    m_waveform = OSC_WAVEFORM_SAW;
    m_damper_pedal = false;
    m_note_number = NOTE_NUMBER_INVALID;
    m_note_hold = false;
    m_output_error = 0;
    IOsc<0>::initialize();
    IFilter<0>::initialize();
    IAmp<0>::initialize();
    IEnvGen<0>::initialize();
    IEnvGen<1>::initialize();
    set_env_decay(0);
    set_amp_env_sus(0);
  }

  INLINE static void set_waveform(uint8_t controller_value) {
    uint8_t waveform;
    if (controller_value < 64) {
      waveform = OSC_WAVEFORM_SAW;
    } else {
      waveform = OSC_WAVEFORM_SQ;
    }

    if (m_waveform != waveform) {
      m_waveform = waveform;
      IOsc<0>::set_waveform(m_waveform);
      if (m_note_number != NOTE_NUMBER_INVALID) {
        IOsc<0>::note_on(m_note_number);
      }
    }

    if (controller_value < 32) {
      IOsc<0>::set_sub((31 - controller_value) << 2);
    } else if (controller_value >= 96) {
      IOsc<0>::set_sub((controller_value - 96) << 2);
    } else {
      IOsc<0>::set_sub(0);
    }
  }

  INLINE static void note_on(uint8_t note_number, uint8_t velocity) {
#if defined(TRANSPOSE)
    if ((note_number < NOTE_NUMBER_MIN - TRANSPOSE) ||
        (note_number > NOTE_NUMBER_MAX - TRANSPOSE)) {
      return;
    }
#else
    if ((note_number < NOTE_NUMBER_MIN) ||
        (note_number > NOTE_NUMBER_MAX)) {
      return;
    }
#endif

    m_note_number = note_number;
    m_note_hold = false;
    IOsc<0>::note_on(note_number);
    IEnvGen<0>::note_on();
    IEnvGen<1>::note_on();
  }

  INLINE static void note_off(uint8_t note_number) {
    if (m_note_number == note_number) {
      if (m_damper_pedal) {
        m_note_hold = true;
      } else {
        all_note_off();
      }
    }
  }

  INLINE static void all_note_off() {
    m_note_number = NOTE_NUMBER_INVALID;
    m_note_hold = false;
    IEnvGen<0>::note_off();
    IEnvGen<1>::note_off();
  }

  INLINE static void control_change(uint8_t controller_number, uint8_t controller_value) {
    switch (controller_number) {
    case FILTER_CUTOFF:
      IFilter<0>::set_cutoff(controller_value);
      break;
    case FILTER_RESO:
      IFilter<0>::set_resonance(controller_value);
      break;
    case FILTER_EG_AMT:
      IFilter<0>::set_env_amt(controller_value);
      break;
    case OSC_MODE:
      // TODO
      break;
    case OSC_COLOR:
      set_waveform(controller_value);
      break;
    case MOD_RATE:
      IOsc<0>::set_detune(controller_value);
      break;
    case MOD_DEPTH:
      IOsc<0>::set_mix(controller_value);
      break;
    case FLUCTUATION:
      IOsc<0>::set_detune_noise_gen_amt(controller_value);
      break;
    case PORTAMENTO:
      IOsc<0>::set_portamento(controller_value);
      break;
    case FILTER_EG:
      set_env_decay(controller_value);
      break;
    case AMP_EG:
      set_amp_env_sus(controller_value);
      break;
#if 0
    case DAMPER_PEDAL:
      if (controller_value < 64) {
        set_damper_pedal(false);
      } else {
        set_damper_pedal(true);
      }
      break;
#endif
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

  INLINE static int8_t clock() {
    m_count++;

    int16_t osc_output = IOsc<0>::clock(m_count);
    uint8_t env_gen_output_0 = IEnvGen<0>::clock(m_count);
    int16_t filter_output = IFilter<0>::clock(m_count, osc_output, env_gen_output_0);
    uint8_t env_gen_output_1 = IEnvGen<1>::clock(m_count);
    int16_t amp_output = IAmp<0>::clock(filter_output, env_gen_output_1);

    // error diffusion
    int16_t output = amp_output + m_output_error;
    m_output_error = low_byte(output);
    return high_sbyte(output);
  }

private:
  INLINE static void set_env_decay(uint8_t controller_value) {
    m_eg0_decay_sustain = controller_value;
    if (m_eg0_decay_sustain < 64) {
      IEnvGen<0>::set_decay(m_eg0_decay_sustain << 1);
      IEnvGen<0>::set_sustain(false);
    } else {
      IEnvGen<0>::set_decay((127 - m_eg0_decay_sustain) << 1);
      IEnvGen<0>::set_sustain(true);
    }
  }

  INLINE static void set_amp_env_sus(uint8_t controller_value) {
    m_eg1_decay_sustain = controller_value;
    if (m_eg1_decay_sustain < 64) {
      IEnvGen<1>::set_decay(m_eg1_decay_sustain << 1);
      IEnvGen<1>::set_sustain(false);
    } else {
      IEnvGen<1>::set_decay((127 - m_eg1_decay_sustain) << 1);
      IEnvGen<1>::set_sustain(true);
    }
  }

  INLINE static void set_damper_pedal(uint8_t on) {
    if (on) {
      m_damper_pedal = true;
    } else {
      if (m_damper_pedal) {
        m_damper_pedal = false;
        turn_hold_off();
      }
    }
  }

  INLINE static void turn_hold_off() {
    m_note_number = NOTE_NUMBER_INVALID;
    m_note_hold = false;
    IEnvGen<0>::note_off();
    IEnvGen<1>::note_off();
  }
};

template <uint8_t T> uint8_t Voice<T>::m_count;
template <uint8_t T> uint8_t Voice<T>::m_waveform;
template <uint8_t T> uint8_t Voice<T>::m_eg0_decay_sustain;
template <uint8_t T> uint8_t Voice<T>::m_eg1_decay_sustain;
template <uint8_t T> boolean Voice<T>::m_damper_pedal;
template <uint8_t T> uint8_t Voice<T>::m_note_number;
template <uint8_t T> boolean Voice<T>::m_note_hold;
template <uint8_t T> uint8_t Voice<T>::m_output_error;
