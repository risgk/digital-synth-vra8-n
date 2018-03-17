require_relative 'DigitalSynthVRA8N/constants'

$file = File.open("sample-midi-stream.bin", "wb")

def control_change(control_number, value)
  $file.write([(CONTROL_CHANGE | MIDI_CH), control_number, value].pack("C*"))
end

def note_on(note_number, velocity)
  $file.write([(NOTE_ON  | MIDI_CH), note_number, velocity].pack("C*"))
end

def note_off(note_number)
  $file.write([(NOTE_OFF | MIDI_CH), note_number, 64].pack("C*"))
end

def wait(length)
  length.times { $file.write([ACTIVE_SENSING].pack("C")) }
end

def play_a(oct)
  play_triad_a(12, 16, 19, oct, 64)
  play_triad_a(16, 19, 23, oct, 32)
  play_triad_a(14, 17, 21, oct, 64)
  play_triad_a(17, 21, 24, oct, 96)
end

def play_b(oct)
  play_triad_b(12, 16, 19, oct, 64)
  play_triad_b(16, 19, 23, oct, 32)
  play_triad_b(14, 17, 21, oct, 64)
  play_triad_b(17, 21, 24, oct, 96)
end

def play_triad_a(x, y, z, oct, velocity)
  note_on(x + (oct * 12), velocity)
  note_on(y + (oct * 12), velocity)
  note_on(z + (oct * 12), velocity)
  wait(5000)
  note_off(x + (oct * 12))
  note_off(y + (oct * 12))
  note_off(z + (oct * 12))
  wait(1250)
end

def play_triad_b(x, y, z, oct, velocity)
  note_on(x + (oct * 12), velocity)
  wait(1250)
  note_on(y + (oct * 12), velocity)
  wait(1250)
  note_on(z + (oct * 12), velocity)
  wait(5000)
  note_off(z + (oct * 12))
  wait(2500)
  note_off(y + (oct * 12))
  wait(2500)
  note_off(x + (oct * 12))
  wait(1250)
end

def sound_off
  control_change(ALL_NOTES_OFF, 0  )
  wait(1250)
end

sound_off

control_change(OSC_COLOR_1  , 0  )
control_change(OSC_COLOR_2  , 64 )
control_change(MOD_RATE     , 8  )
control_change(MOD_DEPTH    , 64 )

control_change(FILTER_CUTOFF, 64 )
control_change(FILTER_RESO  , 64 )
control_change(FILTER_EG_AMT, 96 )
control_change(FILTER_EG    , 32 )

control_change(CC24         , 0  )
control_change(FLUCTUATION  , 64 )
control_change(PORTAMENTO   , 0  )
control_change(AMP_EG       , 120)

control_change(CC28         , 0  )
control_change(CC29         , 0  )
control_change(CC30         , 0  )
control_change(CC31         , 0  )

play_a(4)
play_b(3)

sound_off

$file.close
