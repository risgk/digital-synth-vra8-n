#pragma once

// #define private public  // for tests

#include "common.h"

// associations of units
#define IOsc        Osc
#define IFilter     Filter
#define IAmp        Amp
#define IEnvGen     EnvGen
#define IVoice      Voice
#define ISynthCore  SynthCore

#include "osc.h"
#include "filter.h"
#include "amp.h"
#include "env-gen.h"
#include "voice.h"
#include "synth-core.h"

template <uint8_t T>
class Synth {
public:
  INLINE static void initialize() {
    ISynthCore<0>::initialize();

    // DEFAULT
    ISynthCore<0>::control_change(FILTER_CUTOFF, 96 );
    ISynthCore<0>::control_change(FILTER_RESO  , 64 );
    ISynthCore<0>::control_change(CUTOFF_EG_AMT, 96 );
    ISynthCore<0>::control_change(EG_DECAY     , 32 );

    ISynthCore<0>::control_change(OSC2_PITCH   , 64 );
    ISynthCore<0>::control_change(OSC2_DETUNE  , 16 );
    ISynthCore<0>::control_change(PORTAMENTO   , 0  );
    ISynthCore<0>::control_change(EG_ATTACK    , 32 );

    ISynthCore<0>::control_change(OSC_WAVE     , 0  );
    ISynthCore<0>::control_change(OSC2_ON      , 127);
    ISynthCore<0>::control_change(SUB_OSC_ON   , 127);
    ISynthCore<0>::control_change(EG_SUSTAIN   , 127);

    ISynthCore<0>::control_change(OSC2_P5TH    , 0  );
    ISynthCore<0>::control_change(OSC2_POCT    , 0  );
    ISynthCore<0>::control_change(LEGATO       , 0  );
    ISynthCore<0>::control_change(AMP_EG_ON    , 127);

    ISynthCore<0>::control_change(LFO_RATE     , 64 );
    ISynthCore<0>::control_change(LFO_DEPTH    , 16 );
    ISynthCore<0>::control_change(PITCH_LFO_AMT, 68 );
    ISynthCore<0>::control_change(CO_VEL_AMT   , 0  );

    ISynthCore<0>::control_change(PB_M_RANGE   , 2  );
    ISynthCore<0>::control_change(PB_P_RANGE   , 2  );
    ISynthCore<0>::control_change(PB_TARGET    , 127);
    ISynthCore<0>::control_change(NOTE_ASSIGN  , 0  );
  }

  INLINE static void receive_midi_byte(uint8_t b) {
    ISynthCore<0>::receive_midi_byte(b);
  }

  INLINE static int8_t clock() {
    return ISynthCore<0>::clock();
  }
};
