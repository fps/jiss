require "jissing"
e = jiss.engine()

e:run("notes = range(20, 80, maj7s11(C(4)))")

tick = e:get_samplerate()/8

seq = jiss.sequence(e, "s1")
seq:connect("ardour:MIDI 1/midi_in 1")

seq:insert(0, jiss.lua_event("if math.random() > 0.8 then note_on(0, notes[math.random(#notes)], 127) end;"))
seq:insert(tick, jiss.lua_event("s:relocate(0)"))

e:append(seq)
e:at(0):start()
seq2 = jiss.sequence(e, "s2")
seq2:connect("ardour:MIDI 2/midi_in 1")

seq2:insert(0, jiss.lua_event("if math.random() > 0.8 then note_on(0, notes[math.random(#notes)], 127) end;"))
seq2:insert(tick, jiss.lua_event("s:relocate(0)"))


e:append(seq2)
e:at(1):start()

play(lua_seq(e, "seq3", lines(tick, 
										[[
												note_on(0, C(3), 127)




												note_on(0, G(3)-1, 127)


												s:relocate(0)
										]])))

e:at(2):connect("ardour:MIDI 3/midi_in 1")
-- 
e:start()

-- wait until the user presses a key
io.stdin:read'*l'

