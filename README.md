# Digital Synth VRA8-N v1.x.0

- 201x-xx-xx ISGK Instruments
- <https://github.com/risgk/digital-synth-vra8-n>


## Concept

- Monophonic Synthesizer (MIDI Sound Module) for Arduino Uno


## History

- v1.x
    - Change the layout of VRA8-N CTRL
    - Add "EG > PITCH(-/+)", "EG > P. TGT (2/1&2)", and "RELEASE (OFF/ON)".
    - Add "EG > LFO RATE (-/+)" and "EG > LFO DPT. (-/+)".
    - Change "OSC2 MIX" to "OSC MIX (1/2)"
    - Change "AMP (GATE+RLS/EG)" to "AMP MODE (GATE/EG)"
    - Change "SUSTAIN" to "SUSTAIN (OFF/ON)"
    - Change "LFO WAVE (TRI/SQ)" to "LFO (T/2/S&H/SA/SQ)"
    - Change "LFO > PITCH (2/1&2)" to "LFO > PITCH (-/+)" and "LFO > P. TGT (2/1&2)"
    - Change "KEY ASGN (LO/LAST)" to "K. ASN (L/L/P/H/LST)"
- v1.1
    - Add "SUB WAVE (SIN/SQ)" and "LFO WAVE (TRI/SQ)"

## Features

- Sampling Rate: 31.25 kHz, Bit Depth: 8 bit, LPF Attenuation Slope: -12 dB/oct
- Serial MIDI In (38.4 kbps), PWM Audio Out (Pin 6), PWM Rate: 62.5 kHz
    - We recommend adding a RC filter circuit to reduce PWM ripples
        - A cutoff frequency 15.9 kHz (R: 100 ohm, C: 100 nF) works well
    - **CAUTION**: The Arduino PWM audio output is a unipolar Line Out
        - Please connect this to a power amp/a headphone amp (not to a speaker/a headphone directly)
    - **CAUTION**: Click sounds may occur when you connect the audio out to an amp or reset the board
- We recommend [Hairless MIDI<->Serial Bridge](http://projectgus.github.io/hairless-midiserial/) to connect PC
    - A MIDI Shield (MIDI Breakout) and a power supply adapter are desirable to avoiding USB noise
        - Edit `SERIAL_SPEED` in `DigitalSynthVRA8N.ino` to use MIDI Shield
- Files
    - `DigitalSynthVRA8N.ino` is a sketch for Arduino (Genuino) Uno Rev3
    - `make-sample-wav-file.cc` is for Debugging on PC
        - Requiring GCC (g++) or other
        - `make-sample-wav-file-cc.bat` makes a sample WAV file (working on Windows)
    - `generate-*.rb` generates source files
        - Requiring a Ruby execution environment
- We recommend Arduino IDE 1.8.5

## VRA8-N CTRL

- MIDI Controller (Editor) for VRA8-N, Web App
- VRA8-N CTRL converts Program Changes (#0-7 for PRESET) into Control Changes
- VRA8-N CTRL memorizes USER Programs (#8-15)
- We recommend Google Chrome, which implements Web MIDI API
- We recommend [loopMIDI](http://www.tobias-erichsen.de/software/loopmidi.html) (virtual loopback MIDI cable) to connect VRA8-N
- **CAUTION**: Low CUTOFF with high RESONANCE can damage the speakers
- **CAUTION**: Click sounds may occur when you change the control values

## Details of Controllers

- "LFO (T/2/S&H/SA/SQ)" = LFO WAVE
    - Values 0-15: TRIANGLE1 (Async)
    - Values 16-47: TRIANGLE2 (Sync)
    - Values 48-79: SAMPLE AND HOLD (Sync)
    - Values 80-111: SAW DOWN (Sync)
    - Values 112-127: SQUARE UP (Sync)
- "K. ASN (L/L/P/H/LST)" = KEY ASSIGN
    - Values 0-47: LOWEST Note
    - Values 48-79: PARAPHONIC (Lowest and Highest Notes)
    - Values 80-111: HIGHEST Note
    - Values 112-127: LAST One Note

## A Sample Setting of a Physical Controller (8-Knob)

    +-------------------+---------------+---------------+---------------+
    | CC #16            | CC #17        | CC #23        | CC #19        |
    | CUTOFF            | RESONANCE     | ATTACK        | DECAY         |
    +-------------------+---------------+---------------+---------------+
    | CC #24            | CC #25        | CC #26        | CC #22        |
    | OSC WAVE (SAW/SQ) | OSC MIX (1/2) | SUB OSC MIX   | PORTAMENTO    |
    +-------------------+---------------+---------------+---------------+

## MIDI Implementation Chart

      [Monophonic Synthesizer]                                        Date: 201x-xx-xx       
      Model: Digital Synth VRA8-N     MIDI Implementation Chart       Version: 1.x.0         
    +-------------------------------+---------------+---------------+-----------------------+
    | Function...                   | Transmitted   | Recognized    | Remarks               |
    +-------------------------------+---------------+---------------+-----------------------+
    | Basic        Default          | x             | 1             |                       |
    | Channel      Changed          | x             | x             |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | Mode         Default          | x             | 3             |                       |
    |              Messages         | x             | x             |                       |
    |              Altered          | ************* |               |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | Note                          | x             | 0-127         |                       |
    | Number       : True Voice     | ************* | 0-120         |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | Velocity     Note ON          | x             | x             |                       |
    |              Note OFF         | x             | x             |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | After        Key's            | x             | x             |                       |
    | Touch        Ch's             | x             | x             |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | Pitch Bend                    | x             | o             |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | Control                     1 | x             | o             | MODULATION DEPTH      |
    | Change                        |               |               |                       |
    |                            24 | x             | o             | OSC WAVE (SAW/SQ)     |
    |                            25 | x             | o             | OSC MIX (1/2)         |
    |                            20 | x             | o             | OSC2 COARSE (-/+)     |
    |                            21 | x             | o             | OSC2 FINE (-/+)       |
    |                               |               |               |                       |
    |                            29 | x             | o             | SUB WAVE (SIN/SQ)     |
    |                            26 | x             | o             | SUB OSC MIX           |
    |                           104 | x             | x             | EG > PITCH(-/+)       |
    |                           105 | x             | x             | EG > P. TGT (2/1&2)   |
    |                               |               |               |                       |
    |                            16 | x             | o             | CUTOFF                |
    |                            17 | x             | o             | RESONANCE             |
    |                            18 | x             | o             | EG > CUTOFF (-/+)     |
    |                            31 | x             | o             | AMP MODE (GATE/EG)    |
    |                               |               |               |                       |
    |                            23 | x             | o             | ATTACK                |
    |                            19 | x             | o             | DECAY                 |
    |                            27 | x             | o             | SUSTAIN (OFF/ON)      |
    |                            28 | x             | x             | RELEASE (OFF/ON)      |
    |                               |               |               |                       |
    |                            14 | x             | o             | LFO (T/2/S&H/SA/SQ)   |
    |                            80 | x             | o             | LFO RATE              |
    |                            82 | x             | o             | LFO > PITCH (-/+)     |
    |                            83 | x             | o             | LFO > CUTOFF (-/+)    |
    |                               |               |               |                       |
    |                            81 | x             | o             | LFO DEPTH             |
    |                             3 | x             | x             | EG > LFO RATE (-/+)   |
    |                             9 | x             | x             | LFO > P. TGT (2/1&2)  |
    |                            15 | x             | x             | EG > LFO DPT. (-/+)   |
    |                               |               |               |                       |
    |                            85 | x             | o             | P. BEND RANGE         |
    |                            86 | x             | x             | (RESERVED)            |
    |                           106 | x             | x             | (RESERVED)            |
    |                           107 | x             | x             | (RESERVED)            |
    |                               |               |               |                       |
    |                            22 | x             | o             | PORTAMENTO            |
    |                            30 | x             | o             | LEGATO (OFF/ON)       |
    |                            87 | x             | o             | K. ASN (L/L/P/H/LST)  |
    |                            89 | x             | x             | (RESERVED)            |
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
    | Messages     : All Notes OFF  | x             | o 123-127     |                       |
    |              : Active Sense   | x             | x             |                       |
    |              : Reset          | x             | x             |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | Notes                         |                                                       |
    |                               |                                                       |
    +-------------------------------+-------------------------------------------------------+
      Mode 1: Omni On,  Poly          Mode 2: Omni On,  Mono          o: Yes                 
      Mode 3: Omni Off, Poly          Mode 4: Omni Off, Mono          x: No                  
