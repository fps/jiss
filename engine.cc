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

void engine::register_sequence(const sequence &s) {
	//! And also named like the sequence
	SWIG_NewPointerObj(lua_state, &s, SWIG_TypeQuery(lua_state, "sequence*"), 0);
	lua_setglobal(lua_state, s.name.c_str());
}


void engine::run(const std::string &code) {
	//write_blocking_command(boost::bind(exec_lua, lua_state, code.c_str()));
	exec_lua(lua_state, code.c_str());
}


	/**
		Precondition: current_time has to be set to the time corresponding to the 
		first frame in the buffer to process
	*/
	int engine::process(jack_nframes_t frames, void *arg) {
		e = this;

		nframes = frames;

		while(commands.can_read()) {
			//std::cout << "." << std::endl;
			commands.read()();
			acks.write(0);
			--cmds_pending;
		}

		//! clear the sequences midi buffer even if we are not running
		for (unsigned int index = 0; index < sequences->t.size(); ++index) {
			jack_midi_clear_buffer(jack_port_get_buffer(sequences->t[index]->t.port, frames));

		}

		if (state == STOPPED) return 0;

		for (unsigned int frame = 0; frame < nframes; ++frame) {

			for (unsigned int index = 0; index < sequences->t.size(); ++index) {
				//! initially process all sequences
				sequences->t[index]->t.do_process = true;
				sequences->t[index]->t.current_frame_in_buffer = frame;
			}

			unsigned int relocations = 1;			
			while(relocations != 0) {
				relocations = 0;			
				for (unsigned int index = 0; index < sequences->t.size(); ++index) {
					s = &sequences->t[index]->t;

					if (sequences->t[index]->t.do_process) {
						sequences->t[index]->t.current_time_in_buffer = 0;

						//! process() sets do_process to false 
						sequences->t[index]->t.process(1);
					}

					//! but if the sequence was relocated it needs more processing
					//! see meditation.txt
					if (sequences->t[index]->t.do_process)
						++relocations;

					// if (relocations != 0) std::cout << "." << std::endl;
				} 
			} 
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
	luaL_dostring(lua_state, "require \"jissing\"");

	//! Every lua script will have a variable called e in it pointing to the engine object :D
	SWIG_NewPointerObj(lua_state, this, SWIG_TypeQuery(lua_state, "engine*"), 0);
	lua_setglobal(lua_state, "e");

	client = jack_client_open("jiss", JackNullOption, 0);
	jack_set_process_callback(client, ::process, this);
	jack_activate(client);
}


