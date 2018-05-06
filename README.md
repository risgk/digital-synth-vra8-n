# Digital Synth VRA8-N v0.1.1

- 2018-03-21 ISGK Instruments
- <https://github.com/risgk/digital-synth-vra8-n>

## Concept

- Monophonic Synthesizer for Arduino Uno

## Features

- Monophonic Synthesizer, MIDI Sound Module
- Serial MIDI In (38400 bps), PWM Audio Out (Pin 6), PWM Rate: 62500 Hz
    - We recommend adding a RC filter circuit to reduce PWM ripples
    - A cutoff frequency 15.9 kHz (R: 100 ohm, C: 100 nF) works well, too
    - **CAUTION**: Click sounds may occur when you connect the audio out to an amp/a speaker or reset the board
    - **CAUTION**: The Arduino PWM audio output is a unipolar LINE OUT
        - Please connect this to a power amp/a headphone amp (not to a speaker/a headphone directly)
- Sampling Rate: 31250 Hz, Bit Depth: 8 bit, LPF Attenuation Slope: -12 dB/oct
- Recommending [Hairless MIDI<->Serial Bridge](http://projectgus.github.io/hairless-midiserial/) to connect PC
- Files
    - `DigitalSynthVRA8N.ino` is a sketch for Arduino/Genuino Uno
    - `make-sample-wav-file.cc` is for Debugging on PC
        - Requiring GCC (G++) or other
        - `make-sample-wav-file-cc.bat` makes a sample WAV file (working on Windows)
    - `generate-*.rb` generate source files
        - Requiring a Ruby execution environment
- **CAUTION**: We recommend Arduino IDE 1.8.3

## VRA8-N CTRL

- Parameter Editor (MIDI Controller) for VRA8-N, Web App
- We recommend Google Chrome, which implements Web MIDI API
- VRA8-N CTRL includes PRESET programs
- Recommending [loopMIDI](http://www.tobias-erichsen.de/software/loopmidi.html) (virtual loopback MIDI cable) to connect VRA8-N
- **CAUTION**: Click sounds may occur when you change the controllers
- **CAUTION**: Low CUTOFF with high RESONANCE can damage the speakers

## MIDI Implementation Chart

      [Monophonic Synthesizer]                                        Date: 2018-03-21       
      Model: Digital Synth VRA8-N     MIDI Implementation Chart       Version: 0.1.1         
    +-------------------------------+---------------+---------------+-----------------------+
    | Function...                   | Transmitted   | Recognized    | Remarks               |
    +-------------------------------+---------------+---------------+-----------------------+
    | Basic        Default          | x             | 1             |                       |
    | Channel      Changed          | x             | x             |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | Mode         Default          | x             | Mode 3        |                       |
    |              Messages         | x             | x             |                       |
    |              Altered          | ************* |               |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | Note                          | x             | 0-127         |                       |
    | Number       : True Voice     | ************* | 0-120         |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | Velocity     Note ON          | x             | o (V=1-127)   |                       |
    |              Note OFF         | x             | x             |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | After        Key's            | x             | x             |                       |
    | Touch        Ch's             | x             | x             |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | Pitch Bend                    | x             | o             | Range: 12 (or 2)      |
    +-------------------------------+---------------+---------------+-----------------------+
    | Control                     1 | x             | o             | MODULATION            |
    | Change                     16 | x             | o             | OSC WAVE (SAW/SQ)     |
    |                            17 | x             | o             | SUB OSC (TRI)         |
    |                            18 | x             | o             | OSC2 PITCH            |
    |                            19 | x             | o             | OSC MIX (1/2)         |
    |                            20 | x             | o             | CUTOFF                |
    |                            21 | x             | o             | RESONANCE             |
    |                            22 | x             | o             | FEG AMT (-/+)         |
    |                            23 | x             | o             | FEG DECAY/SUS         |
    |                            24 | x             | o             | ----                  |
    |                            25 | x             | o             | FLUCTUATION           |
    |                            26 | x             | o             | PORTAMENTO            |
    |                            27 | x             | o             | AEG DECAY/SUS         |
    |                            28 | x             | o             | ----                  |
    |                            29 | x             | o             | ----                  |
    |                            30 | x             | o             | VIBRATO RATE          |
    |                            31 | x             | o             | VIBRATO DEPTH         |
    +-------------------------------+---------------+---------------+-----------------------+
    | Program                       | x             | x             |                       |
    | Change       : True #         | ************* |               |                       |
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
