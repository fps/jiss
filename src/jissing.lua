require "jiss"
require "md5"

local get_sequence = 
function (table, key) 
	--- return current sequence when key s is requested
	if "s" == key then
		return e:current_sequence()
	end
	return nil
end

setmetatable(_G, { __index = get_sequence })

--- require "LibCompress/LibCompress"

---- <p> create a named sequence from a table of string events <code> {{t1, e1}, {t2, e2}, ... } </code>
--- which represent lua code. </p>
--- <p>Example:<p/>
--- <p>local tick = e:get_samplerate/4;</p> 
--- local seq1 = lua_seq(e, "seq1", {{0, note_on(0,0,0)}, {tick, note_on(0,0,0)})) </code>
--- @return a lua sequence
function lua_seq(e, name, events)
	local s = jiss.sequence(e, name)
	for i = 1,#events do
		s:insert(events[i][1], jiss.lua_event(events[i][2]))
		end
	return s
end

--- create a named sequence from a table of string events {{t1, e1}, {t2, e2}, ... }
--- which represent cpp code 
function cpp_seq(e, name, preamble, events)
	local s = jiss.sequence(e, name)
	for i = 1,#events do
		e = cpp_event(preamble, events[i][2])
		if nil ~= e then
			s:insert_cpp_event(events[i][1], cpp_event(preamble, events[i][2]))
		end
	end
	return s
end


--- convenience function to add a sequence to the engine and toggle its
--- state to STARTED
function play(s)
	s:start()
	s.e:append(s)
	return s
end

--- add a lua event to sequence table s to relocate to 0 at a certain time t
function loop(time, s)
	local seq = s
	seq:insert_cpp_event(time, cpp_event("", [[s.relocate(0);]]))
	return seq
end

--- expects a string s containing n lines where each line represents
--- code for an event. returns an event table with each
--- event being time t apart
function lines(t, s)
	local time = 0
	local ret = {}
	for line in s:gmatch("[^\n]*\n") do
		--- print("line: " .. line)
		table.insert(ret, {time, line})
		time = time + t
	end
	return ret
end



--- merge two sequence tables
function merge_sequences(s1, s2)
	
end

--- connect all sequences' output to the given port
function connect(e, port)
	for i = 0, (e:num_sequences()-1) do
		e:at(i):connect(port)
	end
end

--- create a event table with evenly spaced times and the given events


--- create a sequence
function midi_sequence(e, name, time, notes)
	local s = jiss.sequence(e, name)
	for i = 1,#notes do
		s:insert((i-1) * time, jiss.lua_event("s:midi_note_on_(0, " .. notes[i] .. ", 127)"))
	end
	return s
end

function cpp_event(preamble, code)
	--- TODO generate UNIQUE name
	local funchash = md5.sumhexa(code)
	local funcname = "run" .. funchash

	--- setup the wrapper code
	local code = 
[[
#include <cstdlib>
#include <cmath>
#include <iostream>
#include "engine.h"
#include "sequence.h"
#include "lv2.h"

]] .. preamble .. [[
			
extern "C" {
  void ]] .. funcname .. [[ () {
  using namespace jiss;

  engine &e = *(engine::get());
  sequence &s = *(e.current_sequence());
]]	.. code .. [[
  }
}
]]
	-- just a comment :D

	sohash = md5.sumhexa(code)
	
	--- TODO handle cleanup in some non-retarded way?

	local filename_base = "/tmp/jiss_" .. sohash 

	if nil == io.open(filename_base .. ".so") then
		print ("compiling cpp code: \n----- \n" .. code .. "\n----")

		io.output(filename_base .. ".cc")
		io.write(code)
		io.flush()

		--- compile the assembled function into object file
		--- TODO: fix up all the things to make this more convenient
		os.execute("g++ -g -fPIC -I. `pkg-config lua5.1 jack lilv-0 --cflags` -o " .. filename_base .. ".so -shared " .. filename_base .. ".cc -Wl,-rpath=. jiss.so")

		os.execute("rm " .. filename_base .. ".cc")
	end

	-- print("cpp_event: " .. filename_base .. ".so  " .. funcname)
	if nil ~= io.open(filename_base .. ".so") then 
		local c =  jiss.cpp_event(filename_base .. ".so", funcname)

		-- os.execute("rm " .. filename_base .. ".so")
		return c
	else
		print("compilation failed")
		return nil
	end
end


--- note names
function C(octave) return 12 * octave end
function D(octave) return 12 * octave + 2 end
function E(octave) return 12 * octave + 4 end
function F(octave) return 12 * octave + 5 end
function G(octave) return 12 * octave + 7 end
function A(octave) return 12 * octave + 9 end
function B(octave) return 12 * octave + 11 end

--- tone collection
function maj7(root) return { root, root + 4, root + 7, root + 11 } end
function maj79(root) return { root, root + 4, root + 7, root + 11, root + 14 } end
function maj7s11(root) return { root, root + 4, root + 7, root + 11, root + 14, root + 18 } end
function min7(root) return { root, root + 3, root + 7, root + 10 } end
function dom7(root) return { root, root + 4, root + 7, root + 10 } end
function dom7s11(root) return { root, root + 4, root + 7, root + 10 , root + 18 } end
function dom7b9b13(root) return { root, root + 4, root + 7, root + 10, root + 13, root + 20 } end
function min7b5(root) return { root, root + 3, root + 6, root + 10 } end
function harmonic_minor(root) return { root, root + 2, root + 3, root + 5, root + 7, root + 8, root + 11} end


--- return an array of notes filled from the scale between min and max
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


--- output a note on message from within a lua_event
function note_on(channel, note, velocity)
	s:midi_note_on(channel, note, velocity)
end

--- output a midi cc message from within a lua_event
function cc(channel, ctrl, val)
	s:midi_cc(channel, ctrl, val)
end

xs = 
	function(tick, p, f, y) 
		ret = {}
		for line in p:gmatch("[^\r\n]+") do
			for i = 1, #line do
				if line:sub(i,i) ~= " " then
					--- print(f(line:sub(i,i), y))
					table.insert(ret, {(i-1)*tick, f(line:sub(i,i), y)})
				end
			end
		end
		return ret
	end

format_xs = 
	function(tick, p, s, t)
		return xs(tick, p, function(x,y) return string.format(s, y[x]) end, t)
	end
