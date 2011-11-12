#include "engine.h"
#include "luarun.h"

extern "C" {
        int process(jack_nframes_t nframes, void *arg) {
                return ((engine*)arg)->process(nframes, arg);
        }
}

engine::engine() :
	commands(1024),
	current_time(0),
	lua_state(luaL_newstate())
{
	SWIG_Lua_NewPointerObj(lua_state, this, SWIG_TypeQueryModule(SWIG_Lua_GetModule(lua_state), SWIG_Lua_GetModule(lua_state), "engine"), 0);

	m = disposable<event_map>::create(event_map());
#if 0
for (unsigned int i = 0; i < 100; ++i) {
console_event c;
c.msg = "blah";
m->t.insert(std::make_pair(44100 * i, disposable<console_event>::create(c)));
}
#endif

	client = jack_client_open("seq++", JackNullOption, 0);
	port = jack_port_register(client, "out0", JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput | JackPortIsTerminal, 0);
	jack_set_process_callback(client, ::process, this);

	jack_activate(client);
}


