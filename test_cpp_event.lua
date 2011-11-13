require "jiss"
require "jissing"

e = jiss.engine()

s = jiss.sequence(e, "s")
s:insert_cpp_event(0.0, cpp_event("s.midi_note_on_(0, 127 * (double)rand()/(double)RAND_MAX, 127);"))
s:insert_cpp_event(0.1, cpp_event("s.relocate(0.0);"))
s:start()

e:append(s)

connect(e,"jass:in")
e:start()

-- wait for the user to press enter
io.stdin:read'*l'
