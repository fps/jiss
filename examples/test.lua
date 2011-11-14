require "jiss"

-- create the engine
e = jiss.engine()


e:run_lua_script("notes = {38, 41, 45, 49, 50, 53, 57, 61}")

-- note that a sequence has to know about the engine it plays on..
seq = jiss.sequence(e, "s1")

seq:connect("jass:in")
seq:insert(0.0, jiss.lua_event("e:at(0):midi_note_on_(0, notes[math.random(#notes)], 127)"))
seq:insert(0.15, jiss.lua_event("e:at(0):relocate(0.0)"))

-- add the sequence to the engine and toggle its state to STARTED
e:append(seq)
e:at(0):start()


seq2 = jiss.sequence(e, "s2")

seq2:connect("jass:in")
seq2:insert(0.0, jiss.lua_event("e:at(1):midi_note_on_(0, 24 + notes[math.random(#notes)], 127)"))
seq2:insert(0.45, jiss.lua_event("e:at(1):relocate(0.0)"))

-- add the sequence to the engine and toggle its state to STARTED
e:append(seq2)
e:at(1):start()


-- start the whole shebang
e:start()

-- wait until the user presses a key
io.stdin:read'*l'

