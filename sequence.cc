#include "sequence.h"

#include "engine.h"

sequence::sequence(engine *e, const std::string &name) : 
	state(STOPPED),
	name(name),
	e(e),
	port(0),
	current_time(0)
{ 
	port = jack_port_register(e->client, name.c_str(), JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput | JackPortIsTerminal, 0);
}

sequence::~sequence() {
	jack_port_unregister(e->client, port);
}


void sequence::connect(const std::string &destinationport) {
	jack_connect(e->client, jack_port_name(port), destinationport.c_str());
}


void sequence::start() {
	e->write_blocking_command(::assign(state, STARTED));
}

void sequence::exec_lua_event(lua_event *l) {
	luaL_dostring(e->lua_state, l->code.c_str());
}


void sequence::process(jack_nframes_t nframes) {
	jack_midi_clear_buffer(jack_port_get_buffer(port, nframes));

	if (state == STOPPED) return;

	// std::cout << "seq:proc" << std::endl;

	double buffer_time = (jiss_time)nframes/(jiss_time)jack_get_sample_rate(e->client);

	current_frame_in_buffer = 0;
	current_time_in_buffer = 0;

	events_map::iterator it = events.lower_bound(current_time);
	// if (it == m->t.end()) std::cout << "end" << std::endl;
	while(true) {
		if ((it == events.end()) || (current_time_in_buffer + (it->first - current_time)) > buffer_time)  {
			current_time += buffer_time - current_time_in_buffer;
			current_time_in_buffer = buffer_time;
			return;
		}

		current_time_in_buffer += it->first - current_time;
		current_time = it->first;

		current_frame_in_buffer = current_time_in_buffer * jack_get_sample_rate(e->client);

		// std::cout << "efhe" << std::endl;

		lua_event *l = dynamic_cast<lua_event*>(it->second.get());
		if (l) {
			//std::cout << "??" << std::endl;
			exec_lua_event(l);
		}

		cpp_event *c = dynamic_cast<cpp_event*>(it->second.get());
		if (c) {
			//std::cout << "??" << std::endl;
			c->o->f();
		}

		events_map::iterator new_it = events.lower_bound(current_time);
		if (it == new_it) ++it;
		else it = new_it;
	}
}
