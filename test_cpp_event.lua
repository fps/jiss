require "jiss"
require "jissing"

e = jiss.engine()

s = jiss.sequence(e, "rand")
s:insert_cpp_event(0.0, cpp_event("s.midi_note_on_(0, 127 * (double)rand()/(double)RAND_MAX, 127);"))
s:insert_cpp_event(0.125, cpp_event("s.relocate(0.0);"))
s:start()

e:append(s)

play_events(e, "bd", {
	{ 0.0,  "s:midi_note_on_(1, 64, 127);s:midi_note_on_(2, 64, 127)" },
	{ 0.125, "s:midi_note_on_(2, 64, 127)" },
	{ 0.25, "s:midi_note_on_(2, 64, math.random(127)" },
	{ 0.375, "s:midi_note_on_(2, 64, math.random(127))" },
	{ 0.5+0.25, "s:relocate(0.0)"}
})

connect(e,"jass:in")
e:start()

-- wait for the user to press enter
io.stdin:read'*l'
