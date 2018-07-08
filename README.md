# Digital Synth VRA8-N v0.8.0

- 2018-07-01 ISGK Instruments
- <https://github.com/risgk/digital-synth-vra8-n>

## Concept

- Monophonic Synthesizer for Arduino Uno

## Features

- Monophonic Synthesizer, MIDI Sound Module
- Sampling Rate: 31.25 kHz, Bit Depth: 8 bit, LPF Attenuation Slope: -12 dB/oct
- Serial MIDI In (38400 bps), PWM Audio Out (Pin 6), PWM Rate: 62500 Hz
    - We recommend adding a RC filter circuit to reduce PWM ripples
        - A cutoff frequency 15.9 kHz (R: 100 ohm, C: 100 nF) works well
    - **CAUTION**: Click sounds may occur when you connect the audio out to an amp/a speaker or reset the board
    - **CAUTION**: The Arduino PWM audio output is a unipolar LINE OUT
        - Please connect this to a power amp/a headphone amp (not to a speaker/a headphone directly)
- We recommend [Hairless MIDI<->Serial Bridge](http://projectgus.github.io/hairless-midiserial/) to connect PC
    - A MIDI Shield (MIDI Breakout) and a power supply adapter are desirable to avoiding USB noise
- Files
    - `DigitalSynthVRA8N.ino` is a sketch for Arduino (Genuino) Uno Rev3
    - `make-sample-wav-file.cc` is for Debugging on PC
        - Requiring GCC (G++) or other
        - `make-sample-wav-file-cc.bat` makes a sample WAV file (working on Windows)
    - `generate-*.rb` generate source files
        - Requiring a Ruby execution environment
    - Edit `SERIAL_SPEED` in `configs.h` to use MIDI Shield
- We recommend Arduino IDE 1.8.3

## VRA8-N CTRL

- Parameter Editor (MIDI Controller) for VRA8-N, Web App
- We recommend Google Chrome, which implements Web MIDI API
- VRA8-N CTRL includes PRESET programs
- We recommend [loopMIDI](http://www.tobias-erichsen.de/software/loopmidi.html) (virtual loopback MIDI cable) to connect VRA8-N
- **CAUTION**: Click sounds may occur when you change the controllers (especially OSC2 MIX and SUB OSC MIX)
- **CAUTION**: Low CUTOFF with high RESONANCE can damage the speakers

## MIDI Implementation Chart

      [Monophonic Synthesizer]                                        Date: 2018-07-01       
      Model: Digital Synth VRA8-N     MIDI Implementation Chart       Version: 0.8.0         
    +-------------------------------+---------------+---------------+-----------------------+
    | Function...                   | Transmitted   | Recognized    | Remarks               |
    +-------------------------------+---------------+---------------+-----------------------+
    | Basic        Default          | x             | 1             |                       |
    | Channel      Changed          | x             | 1-16          | MIDI_CH               |
    +-------------------------------+---------------+---------------+-----------------------+
    | Mode         Default          | x             | 3             |                       |
    |              Messages         | x             | x             |                       |
    |              Altered          | ************* |               |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | Note                          | x             | 0-127         |                       |
    | Number       : True Voice     | ************* | 0-120         |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | Velocity     Note ON          | x             | x 9nH V=1-127 |                       |
    |              Note OFF         | x             | x             |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | After        Key's            | x             | x             |                       |
    | Touch        Ch's             | x             | x             |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | Pitch Bend                    | x             | o             |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | Control                     1 | x             | o             | MODULATION            |
    | Change                     16 | x             | o             | CUTOFF                |
    |                            17 | x             | o             | RESONANCE             |
    |                            18 | x             | o             | CUTOFF < EG (-/+)     |
    |                            19 | x             | o             | DECAY                 |
    |                            20 | x             | o             | OSC2 COARSE (-/+)     |
    |                            21 | x             | o             | OSC2 FINE (-/+)       |
    |                            22 | x             | o             | PORTAMENTO            |
    |                            23 | x             | o             | ATTACK                |
    |                            24 | x             | o             | OSC WAVE (SAW/SQ)     |
    |                            25 | x             | o             | OSC2 MIX              |
    |                            26 | x             | o             | SUB OSC MIX (SIN)     |
    |                            27 | x             | o             | SUSTAIN               |
    |                            28 | x             | x             | (RESERVED)            |
    |                            29 | x             | x             | (RESERVED)            |
    |                            30 | x             | o             | LEGATO (OFF/ON)       |
    |                            31 | x             | o             | AMP GATE+REL/EG       |
    |                            76 | x             | o             | LFO RATE (TRI)        |
    |                            77 | x             | o             | LFO DEPTH             |
    |                            78 | x             | o             | LFO > PITCH (2/1+2)   |
    |                            79 | x             | x             | (RESERVED)            |
    |                            80 | x             | o             | P.BEND RANGE          |
    |                            81 | x             | x             | (RESERVED)            |
    |                            82 | x             | o             | KEY ASGN (LO/LAST)    |
    |                            83 | x             | x             | (RESERVED)            |
    +-------------------------------+---------------+---------------+-----------------------+
    | Program                       | x             | o             |                       |
    | Change       : True #         | ************* | 0-7           |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | System Exclusive              | x             | x             |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | System       : Song Pos       | x             | x             |                       |
    | Common       : Song Sel       | x             | x             |                       |
    |              : Tune           | x             | x             |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | System       : Clock          | x             | x             |                       |
    | Real Time    : Commands       | x             | x             |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | Aux          : Local ON/OFF   | x             | x             |                       |
    | Messages     : All Notes OFF  | x             | o (123-127)   |                       |
    |              : Active Sense   | x             | x             |                       |
    |              : Reset          | x             | x             |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | Notes                         |                                                       |
    |                               |                                                       |
    +-------------------------------+-------------------------------------------------------+
      Mode 1: Omni On,  Poly          Mode 2: Omni On,  Mono          o: Yes                 
      Mode 3: Omni Off, Poly          Mode 4: Omni Off, Mono          x: No                  
