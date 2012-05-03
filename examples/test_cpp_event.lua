require "jiss"
require "jissing"

-- like with all self contained jiss scripts we start by creating the engine
e = jiss.engine()

tick = e:get_samplerate()/8

-- e:exec_cpp_event executes the event in non-RT context, so
-- one can allocate stuff there, etc..
-- note that this may only be done when the engine is stopped
e:exec_cpp_event(cpp_event(
[[
	e.storage_append<int>(0); // create an int entry in the c+ storage
]]
))

-- This sequence consists of a single event that just increases the int at 
-- storage index 0 and then plays a midi note..
play(loop(tick, cpp_seq(e, "rand", lines(1.0, 
[[
	int &i = e.storage_at<int>(0); i = (i + 1) % 128; s.midi_note_on(1, i, 127); 
]]
))))

-- connect all sequence outputs to "jass:in"
connect(e,"jass:in")

-- start the processing
e:start()

-- wait for the user to press enter
io.stdin:read'*l'
