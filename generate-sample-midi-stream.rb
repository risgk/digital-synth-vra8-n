require_relative 'DigitalSynthVRA8N/constants'

$file = File.open("sample-midi-stream.bin", "wb")

def control_change(control_number, value)
  $file.write([(CONTROL_CHANGE | MIDI_CH), control_number, value].pack("C*"))
end

def play(note_number, length)
  $file.write([(NOTE_ON  | MIDI_CH), note_number, 64].pack("C*"))
  (length * 7 / 8).times { $file.write([ACTIVE_SENSING].pack("C")) }
  $file.write([(NOTE_OFF | MIDI_CH), note_number, 64].pack("C*"))
  (length * 1 / 8).times { $file.write([ACTIVE_SENSING].pack("C")) }
end

def wait(length)
  length.times { $file.write([ACTIVE_SENSING].pack("C")) }
end

def play_cegbdfac(c)
  play(12 + (c * 12), 1200)
  play(16 + (c * 12), 1200)
  play(19 + (c * 12), 1200)
  play(23 + (c * 12), 1200)
  play(14 + (c * 12), 1200)
  play(17 + (c * 12), 1200)
  play(21 + (c * 12), 1200)
  play(24 + (c * 12), 6400)
  wait(6400)
end

def sound_off
  control_change(ALL_NOTES_OFF, 0  )
  wait(800)
end

sound_off

control_change(FILTER_CUTOFF, 64 )
control_change(FILTER_RESO  , 96 )
control_change(CUTOFF_EG_AMT, 96 )
control_change(EG_DECAY     , 32 )

control_change(OSC2_COARSE  , 71 )
control_change(OSC2_FINE    , 68 )
control_change(PORTAMENTO   , 0  )
control_change(EG_ATTACK    , 0  )

control_change(OSC_WAVE     , 0  )
control_change(OSC2_ON      , 127)
control_change(SUB_OSC_ON   , 127)
control_change(EG_SUSTAIN   , 0  )

control_change(CC14         , 0  )
control_change(CC15         , 0  )
control_change(LEGATO       , 127)
control_change(AMP_EG_ON    , 0  )

control_change(LFO_RATE     , 64 )
control_change(LFO_DEPTH    , 16 )
control_change(PITCH_LFO_TGT, 0  )
control_change(CC27         , 80 )

control_change(PB_RANGE     , 2  )
control_change(CC29         , 0  )
control_change(NOTE_ASSIGN  , 0  )
control_change(CO_VEL_AMT   , 80 )

play_cegbdfac(3)

sound_off

play_cegbdfac(4)

sound_off

$file.close
