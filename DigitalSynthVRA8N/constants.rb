

A4_PITCH = 440.0

MIDI_CH             = 0
SAMPLING_RATE       = 31250
FREQUENCY_MAX       = 15624
BIT_DEPTH           = 8
NOTE_NUMBER_MIN     = 0
NOTE_NUMBER_MAX     = 120
NOTE_NUMBER_INVALID = 255

OSC_CONTROL_INTERVAL_BITS   = 2
OSC_CONTROL_INTERVAL        = 0x01 << OSC_CONTROL_INTERVAL_BITS
OSC_PHASE_RESOLUTION_BITS   = 24
OSC_TUNE_TABLE_STEPS_BITS   = 8
OSC_TUNE_DENOMINATOR_BITS   = 9
OSC_WAVE_TABLE_AMPLITUDE    = 96
OSC_WAVE_TABLE_SAMPLES_BITS = 8
OSC_DETUNE_MUL_NUM_BITS     = 4
OSC_DETUNE_FREQ_MIN         = 64
OSC_DETUNE_FREQ_MAX         = 0
FILTER_CONTROL_INTERVAL     = 0x08
FILTER_CUTOFF_THROUGH_RATE  = 16
FILTER_TABLE_FRACTION_BITS  = 14
ENV_GEN_CONTROL_INTERVAL    = 0x08
ENV_GEN_LEVEL_MAX           = 0x7F00
ENV_GEN_LEVEL_RELEASE_STEP  = 0x1600
ENV_GEN_ATTACK_FACTOR       = 248
ENV_GEN_DECAY_FACTOR        = 248
AMP_ENV_AMT_MAX             = 240
AMP_ENV_AMT_STEP            = 16

DATA_BYTE_MAX         = 0x7F
STATUS_BYTE_INVALID   = 0x7F
DATA_BYTE_INVALID     = 0x80
STATUS_BYTE_MIN       = 0x80
NOTE_OFF              = 0x80
NOTE_ON               = 0x90
CONTROL_CHANGE        = 0xB0
PITCH_BEND            = 0xE0
SYSTEM_MESSAGE_MIN    = 0xF0
SYSTEM_EXCLUSIVE      = 0xF0
TIME_CODE             = 0xF1
SONG_POSITION         = 0xF2
SONG_SELECT           = 0xF3
TUNE_REQUEST          = 0xF6
EOX                   = 0xF7
REAL_TIME_MESSAGE_MIN = 0xF8
ACTIVE_SENSING        = 0xFE

MODULATION    = 1

FILTER_CUTOFF = 16
FILTER_RESO   = 17
CUTOFF_EG_AMT = 18
EG_DECAY      = 19

OSC2_COARSE   = 20
OSC2_FINE     = 21
PORTAMENTO    = 22
EG_ATTACK     = 23

OSC_WAVE      = 24
OSC2_MIX      = 25
SUB_OSC_MIX   = 26
EG_SUSTAIN    = 27

CC28          = 28
CC29          = 29
LEGATO        = 30
AMP_EG_ON     = 31

LFO_RATE      = 48
LFO_DEPTH     = 49
PITCH_LFO_AMT = 50
CC51          = 51

PB_RANGE      = 56
CC57          = 57
KEY_ASSIGN    = 58
CC59          = 59

ALL_NOTES_OFF = 123
OMNI_MODE_OFF = 124
OMNI_MODE_ON  = 125
MONO_MODE_ON  = 126
POLY_MODE_ON  = 127

OSC_WAVEFORM_SAW     = 0
OSC_WAVEFORM_SQ      = 127
