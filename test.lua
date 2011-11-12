require "jiss"

-- create the engine
e = jiss.engine()

-- note that a sequence has to know about the engine it plays on..
seq = jiss.sequence(e)

seq:insert(0.0, jiss.lua_event("e:midi_note_on_(0, 64, 127)"))
seq:insert(0.2, jiss.lua_event("e:at(0):relocate(0.0)"))

-- in non-RT context it's safe to set the state like this
seq.state = jiss.sequence_STARTED
e:append(seq)

-- start the whole shebang
e:start()

-- wait until the user presses a key
io.stdin:read'*l'

