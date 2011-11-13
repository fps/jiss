require "jiss"
require "jissing"

e = jiss.engine()

s = jiss.sequence(e, "s")
s:connect("midi-monitor:input")

-- s:insert(0.0, jiss.lua_event("print(\"hi\")"))

s:insert_cpp_event(0.0, cpp_event("s.midi_note_on_(0, 64, 127);"))
s:insert_cpp_event(0.1, cpp_event("s.relocate(0.0);"))
s:start()

e:append(s)
e:start()

io.stdin:read'*l'
