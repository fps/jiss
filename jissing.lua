-- two useful functions
function sequence(e, name, events)
	s = jiss.sequence(e, name)
	for i = 1,#events do
		s:insert(events[i][1], jiss.lua_event(events[i][2]))
		end
	return s
end

function play(e, name, events)
	s = sequence(e, name, events)
	s:start()
	s:connect("jass:in")
	e:append(s)
end

