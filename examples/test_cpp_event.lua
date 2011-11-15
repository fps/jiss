require "jiss"
require "jissing"

e = jiss.engine()

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

connect(e,"jass:in")
e:start()

-- wait for the user to press enter
io.stdin:read'*l'
