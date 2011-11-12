#include "engine.h"
#include "luarun.h"

extern "C" {
        int process(jack_nframes_t nframes, void *arg) {
                return ((engine*)arg)->process(nframes, arg);
        }
}


engine::engine() :
	state(STOPPED),
	sequences(gc_sequence_ptr_vector::create(std::vector<gc_sequence_ptr>())),
	commands(1024),
	speed(1.0),
	cmds_pending(0)
{
	lua_state = luaL_newstate();
	luaL_openlibs(lua_state);
	luaL_dostring(lua_state, "require \"jiss\"");

	//! Every lua script will have a variable called e in it :D
	SWIG_NewPointerObj(lua_state, this, SWIG_TypeQuery(lua_state, "engine*"), 0);
	lua_setglobal(lua_state, "e");

	client = jack_client_open("jiss", JackNullOption, 0);

#if 0
	port = jack_port_register(client, "out0", JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput | JackPortIsTerminal, 0);
#endif

	jack_set_process_callback(client, ::process, this);

	jack_activate(client);
}


