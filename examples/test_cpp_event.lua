require "jiss"
require "jissing"

-- like with all self contained jiss scripts we start by creating the engine
e = jiss.engine()

-- e:exec_cpp_event executes the event in non-RT context, so
-- one can allocate stuff there, etc..
-- note that this may only be done when the engine is stopped
e:exec_cpp_event(cpp_event([[
	e.storage->t.push_back(store_base_ptr(new store<int>(64)));
	std::cout << "doing stuff :D" << std::endl;
]]))

s = jiss.sequence(e, "rand")
s:insert_cpp_event(0.0, cpp_event([[
	s.midi_note_on(1, e.storage_at<int>(0), 127);
]]))

s:insert_cpp_event(0.1, cpp_event([[
	s.relocate(0.0);
]]))

s:start()
e:append(s)

-- connect all sequence outputs to "jass:in"
connect(e,"jass:in")

-- start the processing
e:start()

-- wait for the user to press enter
io.stdin:read'*l'
