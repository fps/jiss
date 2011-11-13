-- two useful functions
function sequence(e, name, events)
	local s = jiss.sequence(e, name)
	for i = 1,#events do
		s:insert(events[i][1], jiss.lua_event(events[i][2]))
		end
	return s
end

function play_events(e, name, events)
	local s = sequence(e, name, events)
	s:start()
	s:connect("jass:in")
	e:append(s)
	return sequence
end

function play_sequence(e, s)
	s:start()
	e:append(s)
	return s
end

function loop_sequence(time, s)
	s:insert(time, jiss.lua_event("s:relocate(0.0)"))
	return s
end

function midi_sequence(e, name, time, notes)
	local s = jiss.sequence(e, name)
	for i = 1,#notes do
		s:insert((i-1) * time, jiss.lua_event("s:midi_note_on_(0, " .. notes[i] .. ", 127)"))
	end
	return s
end

function cpp_event(code)
	print ("compiling cpp code: " .. code)
	-- TODO generate UNIQUE name
	local funcname = "run".. math.random(1000000)

	-- setup the wrapper code
	local code = '#include <iostream>\n#include "engine.h"\n#include "sequence.h"\n\nextern "C" {\n  void ' .. funcname .. '() {\n    engine &e = *(engine::get());\n    sequence &s = *(e.current_sequence());\n    ' .. code .. '\n  }\n}\n'
	-- print(code)

	-- TODO handle cleanup in some non-retarded way?
	local filename = os.tmpname()
	io.output(filename .. ".cc")
	io.write(code)
	io.flush()

	-- compile the assembled function into object file
	-- TODO: fix up all the things to make this more convenient
	os.execute("g++ -g -fPIC -I. -I/usr/include/lua5.1 -o " .. filename .. ".so -shared " .. filename .. ".cc -Wl,-rpath=. jiss.so")

	local c =  jiss.cpp_event(filename .. ".so", funcname)
	
	os.execute("rm " .. filename .. ".cc")
	os.execute("rm " .. filename .. ".so")
	return c
end