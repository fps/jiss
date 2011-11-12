#include "engine.h"
#include "luarun.h"

extern "C" {
        int process(jack_nframes_t nframes, void *arg) {
                return ((engine*)arg)->process(nframes, arg);
        }
}

void engine::exec_lua_event(const std::string &code) {
	luaL_dostring(lua_state, code.c_str());
}

void engine::exec_lua_init() {
	luaL_dostring(lua_state, lua_init.c_str());
}

engine::engine() :
	state(STOPPED),
	commands(1024),
	current_time(0),
	current_time_in_buffer(0)
{
	lua_state = luaL_newstate();
	luaL_openlibs(lua_state);
	luaL_dostring(lua_state, "require \"jiss\"");

	//! Every lua script will have a variable called e in it :D
	SWIG_NewPointerObj(lua_state, this, SWIG_TypeQuery(lua_state, "engine*"), 0);
	lua_setglobal(lua_state, "e");
	//luaL_dostring(lua_state, "e.start(e)");

	m = disposable<event_map>::create(event_map());
#if 0
	lua_event l;
	l.code = "e.midi_note_on(e, 0, 64, 127)";
	m->t.insert(std::make_pair(1.0, disposable<lua_event>::create(l)));

	lua_event l2;
	l2.code = "print(\";\"); e.relocate(e, 0.0)";
	m->t.insert(std::make_pair(2.0, disposable<lua_event>::create(l2)));
#endif
	client = jack_client_open("seq++", JackNullOption, 0);
	port = jack_port_register(client, "out0", JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput | JackPortIsTerminal, 0);
	jack_set_process_callback(client, ::process, this);

	jack_activate(client);
}


