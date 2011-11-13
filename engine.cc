#include "engine.h"
#include "luarun.h"

extern "C" {
        int process(jack_nframes_t nframes, void *arg) {
                return ((engine*)arg)->process(nframes, arg);
        }
}

engine *engine::e = 0;

void exec_lua(lua_State *state, const std::string &code) {
	luaL_dostring(state, code.c_str());
}

void engine::run(const std::string &code) {
	write_blocking_command(boost::bind(exec_lua, lua_state, code.c_str()));
}


	/**
		Precondition: current_time has to be set to the time corresponding to the 
		first frame in the buffer to process
	*/
	int engine::process(jack_nframes_t nframes, void *arg) {
		e = this;

		while(commands.can_read()) {
			//std::cout << "." << std::endl;
			commands.read()();
			acks.write(0);
			--cmds_pending;
		}

		for (unsigned int index = 0; index < sequences->t.size(); ++index) {
				jack_midi_clear_buffer(jack_port_get_buffer(sequences->t[index]->t.port, 1024));
		}

		if (state == STOPPED) return 0;

		for (unsigned int index = 0; index < sequences->t.size(); ++index) {
			s = &sequences->t[index]->t;
			//! Every lua script will have a variable called s in it pointing to the current sequence :D
			SWIG_NewPointerObj(lua_state, &sequences->t[index]->t, SWIG_TypeQuery(lua_state, "sequence*"), 0);
			lua_setglobal(lua_state, "s");

			//! And also named like the sequence
			SWIG_NewPointerObj(lua_state, &sequences->t[index]->t, SWIG_TypeQuery(lua_state, "sequence*"), 0);
			lua_setglobal(lua_state, sequences->t[index]->t.name.c_str());

			sequences->t[index]->t.process(nframes);
		}

		return 0;
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

	//! Every lua script will have a variable called e in it pointing to the engine object :D
	SWIG_NewPointerObj(lua_state, this, SWIG_TypeQuery(lua_state, "engine*"), 0);
	lua_setglobal(lua_state, "e");

	client = jack_client_open("jiss", JackNullOption, 0);
	jack_set_process_callback(client, ::process, this);
	jack_activate(client);
}


