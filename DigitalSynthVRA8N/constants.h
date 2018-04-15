#pragma once

const double    A4_PITCH = 440.0;

const uint8_t   MIDI_CH             = 0;
const uint16_t  SAMPLING_RATE       = 31250;
const uint16_t  FREQUENCY_MAX       = 15624;
const uint8_t   BIT_DEPTH           = 8;
const uint8_t   NOTE_NUMBER_MIN     = 24;
const uint8_t   NOTE_NUMBER_MAX     = 84;
const uint8_t   NOTE_NUMBER_INVALID = 255;

const uint8_t   OSC_CONTROL_INTERVAL_BITS   = 3;
const uint8_t   OSC_CONTROL_INTERVAL        = 0x01 << OSC_CONTROL_INTERVAL_BITS;
const uint8_t   OSC_PHASE_RESOLUTION_BITS   = 24;
const uint8_t   OSC_TUNE_TABLE_STEPS_BITS   = 8;
const uint8_t   OSC_TUNE_DENOMINATOR_BITS   = 9;
const uint8_t   OSC_WAVE_TABLE_AMPLITUDE    = 96;
const uint8_t   OSC_WAVE_TABLE_SAMPLES_BITS = 8;
const uint8_t   OSC_DETUNE_MUL_NUM_BITS     = 4;
const uint16_t  OSC_DETUNE_FREQ_MIN         = 64;
const uint16_t  OSC_DETUNE_FREQ_MAX         = 0;
const uint8_t   FILTER_CONTROL_INTERVAL     = 0x08;
const uint8_t   FILTER_CUTOFF_THROUGH_RATE  = 16;
const uint8_t   FILTER_TABLE_FRACTION_BITS  = 14;
const uint8_t   ENV_GEN_CONTROL_INTERVAL    = 0x08;
const uint16_t  ENV_GEN_LEVEL_MAX           = 0xFFFF;
const uint16_t  ENV_GEN_LEVEL_RELEASE_STEP  = 0x1600;
const uint8_t   ENV_GEN_DECAY_FACTOR        = 248;
const uint8_t   AMP_ENV_AMT_MAX             = 240;
const uint8_t   AMP_ENV_AMT_STEP            = 16;

const uint8_t   DATA_BYTE_MAX         = 0x7F;
const uint8_t   STATUS_BYTE_INVALID   = 0x7F;
const uint8_t   DATA_BYTE_INVALID     = 0x80;
const uint8_t   STATUS_BYTE_MIN       = 0x80;
const uint8_t   NOTE_OFF              = 0x80;
const uint8_t   NOTE_ON               = 0x90;
const uint8_t   CONTROL_CHANGE        = 0xB0;
const uint8_t   PITCH_BEND            = 0xE0;
const uint8_t   SYSTEM_MESSAGE_MIN    = 0xF0;
const uint8_t   SYSTEM_EXCLUSIVE      = 0xF0;
const uint8_t   TIME_CODE             = 0xF1;
const uint8_t   SONG_POSITION         = 0xF2;
const uint8_t   SONG_SELECT           = 0xF3;
const uint8_t   TUNE_REQUEST          = 0xF6;
const uint8_t   EOX                   = 0xF7;
const uint8_t   REAL_TIME_MESSAGE_MIN = 0xF8;
const uint8_t   ACTIVE_SENSING        = 0xFE;

const uint8_t   MODULATION    = 1;

const uint8_t   OSC_COLOR_1   = 16;
const uint8_t   OSC_COLOR_2   = 17;
const uint8_t   MOD_RATE      = 18;
const uint8_t   MOD_DEPTH     = 19;

const uint8_t   FILTER_CUTOFF = 20;
const uint8_t   FILTER_RESO   = 21;
const uint8_t   FILTER_EG_AMT = 22;
const uint8_t   FILTER_EG     = 23;

const uint8_t   CC24          = 24;
const uint8_t   FLUCTUATION   = 25;
const uint8_t   PORTAMENTO    = 26;
const uint8_t   AMP_EG        = 27;

const uint8_t   CC28          = 28;
const uint8_t   CC29          = 29;
const uint8_t   CC30          = 30;
const uint8_t   CC31          = 31;

const uint8_t   DAMPER_PEDAL  = 64;

const uint8_t   ALL_NOTES_OFF = 123;
const uint8_t   OMNI_MODE_OFF = 124;
const uint8_t   OMNI_MODE_ON  = 125;
const uint8_t   MONO_MODE_ON  = 126;
const uint8_t   POLY_MODE_ON  = 127;

const uint8_t   OSC_WAVEFORM_SAW     = 0;
const uint8_t   OSC_WAVEFORM_SQ      = 127;
