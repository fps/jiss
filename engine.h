#ifndef SEQPP_E_HH
#define SEQPP_E_HH

#include <jack/jack.h>
#include <map>
#include <boost/function.hpp>
#include <boost/bind.hpp>

extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

#include "disposable.h"
#include "ringbuffer.h"
#include "console_event.h"
#include "lua_event.h"
#include "heap.h"


extern "C" { 
	int process(jack_nframes_t nframes, void *arg); 
}

typedef double jiss_time;

typedef disposable<event> disposable_event;
typedef boost::shared_ptr<disposable<event> > disposable_event_ptr;

typedef std::multimap<jiss_time, disposable_base_ptr> event_map;

typedef boost::shared_ptr<disposable<event_map> > disposable_event_map_ptr;
typedef ringbuffer<boost::function<void(void)> > command_ringbuffer;



struct engine {
	enum {STOPPED, STARTED} state;

	jack_client_t *client;
	jack_port_t *port;

	boost::shared_ptr<disposable<event_map> > m;

	command_ringbuffer commands;

	jiss_time current_time;

	lua_State *lua_state;

	void exec_lua_event(const std::string &code);

	int process(jack_nframes_t nframes, void *arg) {
		while(commands.can_read()) commands.read()();

		event_map::iterator it = m->t.lower_bound(current_time);
		// if (it == m->t.end()) std::cout << "end" << std::endl;

		while(it != m->t.end() && it->first < current_time + (jiss_time)nframes/(jiss_time)jack_get_sample_rate(client)) {
			//std::cout << it->second->t.msg << std::flush;
			disposable<console_event>* c = dynamic_cast<disposable<console_event>*>(it->second.get());
			if (c) {
				std::cout << c->t.msg << std::flush;
			}
			
			disposable<lua_event>* l = dynamic_cast<disposable<lua_event>*>(it->second.get());
			if (l) {
				exec_lua_event(l->t.code);
			}
				++it;
		}

		current_time += (jiss_time)nframes/(jiss_time)jack_get_sample_rate(client);
		return 0;
	}
	
	void midi_note_on(unsigned int channel, unsigned int note, unsigned int velocity) {

	}

	void midi_note_off(unsigned int channel, unsigned int note) {

	}

	void start() {
		commands.write(boost::bind(&engine::start_, this));
	}

	void stop() {
		commands.write(boost::bind(&engine::stop_, this));
	}

	engine();

#if 0
	engine() :
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
#endif

	~engine() {
		jack_client_close(client);
		jack_deactivate(client);
		lua_close(lua_state);
	}


	protected:
		void start_() {
			std::cout << "start" << std::endl;

		}

		void stop_() {
			std::cout << "stop" << std::endl;
		}
};



#endif

