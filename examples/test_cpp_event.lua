require "jiss"
require "jissing"

-- like with all self contained jiss scripts we start by creating the engine
e = jiss.engine()

-- e:exec_cpp_event executes the event in non-RT context, so
-- one can allocate stuff there, etc..
-- note that this may only be done when the engine is stopped
e:exec_cpp_event(cpp_event([[
	e.storage_append<int>(64);
	std::cout << "doing stuff :D" << std::endl;
]]))

s = cpp_seq(e, "rand", lines(1.0, [[
	s.midi_note_on(1, e.storage_at<int>(0), 127);
]]))

s = loop(1.0, s)

s:start()
e:append(s)

-- connect all sequence outputs to "jass:in"
connect(e,"jass:in")

-- start the processing
e:start()

-- wait for the user to press enter
io.stdin:read'*l'
