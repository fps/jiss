require "jiss"

-- create the engine
e = jiss.engine()


e:run_lua_script("bar = 1; stella = {{38, 41, 45, 48, 50, 53, 57, 60, 65}, {45, 49, 52, 55, 57, 61, 64, 67}}")


-- note that a sequence has to know about the engine it plays on..
seq = jiss.sequence(e, "s1")

seq:connect("jass:in")
seq:insert(0.0, jiss.lua_event("for i = 1,1 do s:midi_note_on_(0, stella[bar][math.random(#stella[bar])], 64 + math.random()*64) end"))
seq:insert(2*0.15, jiss.lua_event("e:at(0):relocate(0.0)"))

-- add the sequence to the engine and toggle its state to STARTED
seq:start()
e:append(seq)

seq2 = jiss.sequence(e, "s2")

seq2:connect("jass:in")
seq2:insert(0.0, jiss.lua_event("for i = 1,4 do e:at(1):midi_note_on_(0, 36 + stella[bar][math.random(#stella[bar])], 64 + math.random()*64) end"))
seq2:insert(2*0.45, jiss.lua_event("e:at(1):relocate(0.0)"))

-- add the sequence to the engine and toggle its state to STARTED
e:append(seq2)
e:at(1):start()

-- a sequence that controls the global variable bar to advance through the song
seq3 = jiss.sequence(e, "s3");
seq3:insert(0.0, jiss.lua_event("bar = 1"))
seq3:insert(0.15 * 12, jiss.lua_event("bar = 2"))
seq3:insert(0.15 * 24, jiss.lua_event("e:at(2):relocate(0.0)"))

seq3:start()
e:append(seq3)

-- start the whole shebang
e:start()

-- wait until the user presses a key
io.stdin:read'*l'

