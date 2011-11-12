require "jiss"

-- create the engine
e = jiss.engine()

-- note that a sequence has to know about the engine it plays on..
seq = jiss.sequence(e, "s1")

seq:connect("jass:in")
seq:insert(0.0, jiss.lua_event("e:at(0):midi_note_on_(0, 64, 127)"))
seq:insert(0.1, jiss.lua_event("e:at(0):relocate(0.0)"))

-- add the sequence to the engine and toggle its state to STARTED
e:append(seq)
e:at(0):start()


seq2 = jiss.sequence(e, "s2")

seq2:connect("jass:in")
seq2:insert(0.0, jiss.lua_event("e:at(1):midi_note_on_(0, 84, 127)"))
seq2:insert(0.3, jiss.lua_event("e:at(1):relocate(0.0)"))

-- add the sequence to the engine and toggle its state to STARTED
e:append(seq2)
e:at(1):start()


-- start the whole shebang
e:start()

-- wait until the user presses a key
io.stdin:read'*l'

