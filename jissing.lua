-- two useful functions
function sequence(e, name, events)
	s = jiss.sequence(e, name)
	for i = 1,#events do
		s:insert(events[i][1], jiss.lua_event(events[i][2]))
		end
	return s
end

function play_events(e, name, events)
	s = sequence(e, name, events)
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
	s = jiss.sequence(e, name)
	for i = 1,#notes do
		s:insert((i-1) * time, jiss.lua_event("s:midi_note_on_(0, " .. notes[i] .. ", 127)"))
	end
	return s
end
