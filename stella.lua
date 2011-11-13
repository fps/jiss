require "jiss"
require "jissing"

-- create the engine
e = jiss.engine()

-- setup some state that the sequences later use
e:run_lua_script("bar = 1; stella = {{38, 41, 45, 48, 50, 53, 57, 60, 65}, {45, 49, 52, 55, 57, 61, 64, 67}}")

play(e, "s1", {
	{ 0.0,    "s:midi_note_on_(0, stella[bar][math.random(#stella[bar])], 64 + math.random()*64)"},
	{ 2*0.15, "s:relocate(0.0)"}
})

play(e, "s2", {
	{ 0.0,    "for i = 1,4 do \
		           s:midi_note_on_(0, 36 + stella[bar][math.random(#stella[bar])], 64 + math.random()*64) \
	           end"},
	{ 2*0.45, "s:relocate(0.0)"}
})

-- a sequence that controls the global variable bar to advance through the song
play(e, "s3", {
	{0.0,       "bar = 1"},
	{0.15 * 12, "bar = 2"},
	{0.15 * 24, "s:relocate(0.0)"}
})

-- every 3 seconds distort some notes in stella[2]
play(e, "f",  {
	{0.0, "stella[2][math.random(#stella[2])] = math.random(127)"},
	{3.0, "s:relocate(0.0)"}
})




-- start the whole shebang
e:start()

-- wait until the user presses a key
io.stdin:read'*l'

