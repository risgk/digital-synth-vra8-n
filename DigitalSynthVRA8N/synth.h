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
    ISynthCore<0>::control_change(FILTER_CUTOFF, 80 );
    ISynthCore<0>::control_change(FILTER_RESO  , 80 );
    ISynthCore<0>::control_change(CUTOFF_EG_AMT, 80 );
    ISynthCore<0>::control_change(EG_DECAY     , 32 );

    ISynthCore<0>::control_change(OSC2_COARSE  , 71 );
    ISynthCore<0>::control_change(OSC2_FINE    , 68 );
    ISynthCore<0>::control_change(PORTAMENTO   , 0  );
    ISynthCore<0>::control_change(EG_ATTACK    , 0  );

    ISynthCore<0>::control_change(OSC_WAVE     , 0  );
    ISynthCore<0>::control_change(CC14         , 0  );
    ISynthCore<0>::control_change(CC15         , 0  );
    ISynthCore<0>::control_change(EG_SUSTAIN   , 127);

    ISynthCore<0>::control_change(OSC2_MIX     , 127);
    ISynthCore<0>::control_change(SUB_OSC_MIX  , 127);
    ISynthCore<0>::control_change(LEGATO       , 127);
    ISynthCore<0>::control_change(AMP_EG_ON    , 127);

    ISynthCore<0>::control_change(LFO_RATE     , 64 );
    ISynthCore<0>::control_change(LFO_DEPTH    , 16 );
    ISynthCore<0>::control_change(PITCH_LFO_AMT, 66 );
    ISynthCore<0>::control_change(CC51         , 0  );

    ISynthCore<0>::control_change(PB_RANGE     , 2  );
    ISynthCore<0>::control_change(KEY_ASSIGN   , 0  );
    ISynthCore<0>::control_change(CC58         , 0  );
    ISynthCore<0>::control_change(CC59         , 0  );
  }

  INLINE static void receive_midi_byte(uint8_t b) {
    ISynthCore<0>::receive_midi_byte(b);
  }

  INLINE static int8_t clock() {
    return ISynthCore<0>::clock();
  }
};
