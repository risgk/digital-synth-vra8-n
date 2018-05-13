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
    ISynthCore<0>::control_change(OSC_COLOR_1  , 0  );
    ISynthCore<0>::control_change(OSC_COLOR_2  , 0  );
    ISynthCore<0>::control_change(MOD_RATE     , 67 );
    ISynthCore<0>::control_change(MOD_DEPTH    , 64 );

    ISynthCore<0>::control_change(FILTER_CUTOFF, 64 );
    ISynthCore<0>::control_change(FILTER_RES   , 64 );
    ISynthCore<0>::control_change(FILTER_EG_AMT, 96 );
    ISynthCore<0>::control_change(FILTER_EG    , 64 );

    ISynthCore<0>::control_change(CC24         , 0  );
    ISynthCore<0>::control_change(CC25         , 127);
    ISynthCore<0>::control_change(PORTAMENTO   , 0  );
    ISynthCore<0>::control_change(AMP_EG       , 16 );

    ISynthCore<0>::control_change(CC28         , 0  );
    ISynthCore<0>::control_change(CC29         , 0  );
    ISynthCore<0>::control_change(LFO_RATE     , 64 );
    ISynthCore<0>::control_change(LFO_DEPTH    , 0  );
  }

  INLINE static void receive_midi_byte(uint8_t b) {
    ISynthCore<0>::receive_midi_byte(b);
  }

  INLINE static int8_t clock() {
    return ISynthCore<0>::clock();
  }
};
