require "jiss"

-- create a named sequence from a table of events {{t1, e1}, {t2, e2}, ... }
function seq(e, name, events)
	local s = jiss.sequence(e, name)
	for i = 1,#events do
		s:insert(events[i][1], jiss.lua_event(events[i][2]))
		end
	return s
end

-- convenience function to add a sequence to the engine and toggle its
-- state to STARTED
function play(s)
	s:start()
	s.e:append(s)
	return s
end

-- add a lua event to sequence table s to relocate to 0 at a certain time t
function loop_events(time, s)
	local seq = s
	table.insert(seq, {time, [[s:relocate(0.0)]]})
	return seq
end

-- expects a string s containing n lines which contain
-- lua chunks. returns an event table with each
-- event being time t apart
function events_string(t, s)
	local time = 0
	local ret = {}
	for line in s:gmatch("[^\n]*") do
		table.insert(ret, {time, line})
		time = time + t
	end
	return ret
end



-- merge two sequence tables
function merge_sequences(s1, s2)
	
end

-- connect all sequences' output to the given port
function connect(e, port)
	for i = 0, (e:num_sequences()-1) do
		e:at(i):connect(port)
	end
end

-- create a event table with evenly spaced times and the given events


-- create a sequence
function midi_sequence(e, name, time, notes)
	local s = jiss.sequence(e, name)
	for i = 1,#notes do
		s:insert((i-1) * time, jiss.lua_event("s:midi_note_on_(0, " .. notes[i] .. ", 127)"))
	end
	return s
end

function cpp_event(code)
	print ("compiling cpp code: \n" .. code)
	-- TODO generate UNIQUE name
	local funcname = "run".. math.random(1000000)

	-- setup the wrapper code
	local code = '#include <cstdlib>\n#include<cmath>\n#include <iostream>\n#include "engine.h"\n#include "sequence.h"\n\nextern "C" {\n  void ' .. funcname .. '() {\n    engine &e = *(engine::get());\n    sequence &s = *(e.current_sequence());\n    ' .. code .. '\n  }\n}\n'
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


-- note names
function C(octave)
	return 12 * octave
end

-- note names
function D(octave)
	return 12 * octave + 2
end

-- note names
function E(octave)
	return 12 * octave + 4
end

-- note names
function F(octave)
	return 12 * octave + 5
end

-- note names
function G(octave)
	return 12 * octave + 7
end

-- note names
function A(octave)
	return 12 * octave + 9
end

-- note names
function B(octave)
	return 12 * octave + 11
end

-- tone collection
function maj7(root) 
	return { root, root + 4, root + 7, root + 11 }
end

function maj79(root) 
	return { root, root + 4, root + 7, root + 11, root + 14 }
end

function maj7s11(root) 
	return { root, root + 4, root + 7, root + 11, root + 14, root + 18 }
end


function min7(root)
	return { root, root + 3, root + 7, root + 10 }
end

function dom7(root)
	return { root, root + 4, root + 7, root + 10 }
end

function dom7s11(root)
	return { root, root + 4, root + 7, root + 10 , root + 18 }
end


function dom7b9b13(root)
	return { root, root + 4, root + 7, root + 10, root + 13, root + 20 }
end


function min7b5(root)
	return { root, root + 3, root + 6, root + 10 }
end

function harmonic_minor(root)
	return { root, root + 2, root + 3, root + 5, root + 7, root + 8, root + 11}
end


-- return an array of notes filled from the scale between min and max
function range(min, max, scale)
	local c = 1
	local ret = {};
	for i = min,max do
		for j = 1,#scale do
			if ((scale[j] % 12) == (i % 12)) then 
				ret[c] = i
				c = c + 1;
			end
		end
	end
	return ret;
end


-- output a note on message from within a lua_event
function note_on(channel, note, velocity)
	s:midi_note_on(channel, note, velocity)
end

-- output a midi cc message from within a lua_event
function cc(channel, ctrl, val)
	s:midi_cc(channel, ctrl, val)
end

