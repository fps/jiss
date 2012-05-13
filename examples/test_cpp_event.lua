require "jissing"

-- like with all self contained jiss scripts we start by creating the engine
e = jiss.engine()

-- os.execute("sleep 2")

tick = e:get_samplerate()/20

pre = [[
#include "jissing.h"
using namespace jissing;
]]

-- e:exec_cpp_event executes the event in non-RT context, so
-- one can allocate stuff there, etc..
-- note that this may only be done when the engine is stopped
e:exec_cpp_event(cpp_event(pre,
[[
  e.storage_append<int>(0); // create an int entry in the c+ storage
  e.storage_append(
    synth(e.get_samplerate(), (float)e.get_buffersize(), 
    n<0>(), n<0>(), n<1>(), 
    sin(_const(440), _const(0.0))));
]]
))

-- This sequence consists of a single event that just increases the int at 
-- storage index 0 and then plays a midi note..
play(loop(tick, cpp_seq(e, "rand", pre, lines(1.0, 
[[
  int &i = e.storage_at<int>(0); i = (i + 1) % 128; s.midi_note_on(1, i, 127); 
]]
))))

-- connect all sequence outputs to "jass:in"
connect(e,"ardour:MIDI 1/midi_in 1")

-- start the processing
e:start()

-- wait for the user to press enter
io.stdin:read'*l'
