require "jiss"
e = jiss.engine()
e:start()
seq = jiss.sequence()
seq:insert_lua_event(0.0, jiss.lua_event("e.midi_note_on(0, 64, 127)"))
seq:insert_lua_event(1.0, jiss.lua_event("print(\"hi\"); e.relocate(e, 0.0)"))
io.stdin:read'*l'

