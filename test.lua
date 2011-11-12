require "jiss"

-- create the engine
e = jiss.engine()

-- note that a sequence has to know about the engine it plays on..
seq = jiss.sequence(e)

-- seq:clear()
seq:insert(0.0, jiss.lua_event("e.midi_note_on(0, 64, 127)"))
seq:insert(1.0, jiss.lua_event("print(\"hi\"); e.relocate(e, 0.0)"))
seq:start()
e:append(seq)

-- start the whole shebang
e:start()

-- wait until the user presses a key
io.stdin:read'*l'

