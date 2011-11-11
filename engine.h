#ifndef SEQPP_E_HH
#define SEQPP_E_HH

#include <jack/jack.h>
#include <map>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "disposable.h"
#include "ringbuffer.h"
#include "console_event.h"
#include "heap.h"


extern "C" { int process(jack_nframes_t nframes, void *arg); }

typedef disposable<event> disposable_event;
typedef boost::shared_ptr<disposable<event> > disposable_event_ptr;
typedef std::multimap<jack_nframes_t, disposable_base_ptr> event_map;
typedef boost::shared_ptr<disposable<event_map> > disposable_event_map_ptr;
typedef ringbuffer<boost::function<void(void)> > command_ringbuffer;


struct engine {
	jack_client_t *client;
	jack_port_t *port;

	boost::shared_ptr<disposable<event_map> > m;

	command_ringbuffer commands;

	jack_nframes_t current_frame;

	lua_State *lua_state;

	int process(jack_nframes_t nframes, void *arg) {
		while(commands.can_read()) commands.read()();

		event_map::iterator it = m->t.lower_bound(current_frame);
		// if (it == m->t.end()) std::cout << "end" << std::endl;

		while(it != m->t.end() && it->first < current_frame + nframes) {
			//std::cout << it->second->t.msg << std::flush;
			disposable<console_event>* c = dynamic_cast<disposable<console_event>*>(it->second.get());
			if (c) {
				std::cout << c->t.msg << std::flush;
			}
			
			++it;
		}

		current_frame += nframes;
		return 0;
	}
	

	~engine() {
		jack_client_close(client);
		jack_deactivate(client);
		lua_close(lua_state);
	}

	engine() :
		commands(1024),
		current_frame(0),
		lua_state(lua_open())
	{
		m = disposable<event_map>::create(event_map());
		for (unsigned int i = 0; i < 100; ++i) {
			console_event c;
			c.msg = "blah";
			m->t.insert(std::make_pair(44100 * i, disposable<console_event>::create(c)));
		}
	
		client = jack_client_open("seq++", JackNullOption, 0);
		port = jack_port_register(client, "out0", JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput | JackPortIsTerminal, 0);
		jack_set_process_callback(client, ::process, this);

		jack_activate(client);
	}
};



#endif

