#include "sequence.h"

#include "engine.h"

sequence::sequence(engine *e, const std::string &name) : 
	state(STOPPED),
	name(name),
	e(e),
	port(0),
	current_time(0),
	do_process(false)
{ 
	s_bind = "s = " + name;
	port = jack_port_register(e->client, name.c_str(), JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput | JackPortIsTerminal, 0);
}

sequence::~sequence() {
	jack_port_unregister(e->client, port);
}


void sequence::connect(const std::string &destinationport) {
	jack_connect(e->client, jack_port_name(port), destinationport.c_str());
}

void sequence::start_() { 
	if (state == STOPPED) {
		state = STARTED; 
	}
	state = STARTED; 
}


void sequence::start() {
	e->write_blocking_command(::assign(state, STARTED));
}

void sequence::stop() {
	e->write_blocking_command(::assign(state, STOPPED));
}

void sequence::exec_cpp_event(cpp_event *c) {
	c->o->f();
}

void sequence::exec_lua_event(lua_event *l) {
	luaL_dostring(e->lua_state, l->code.c_str());
}


void sequence::process(jack_nframes_t nframes) {
	do_process = false;
	// jack_midi_clear_buffer(jack_port_get_buffer(port, nframes));

	if (state == STOPPED) return;

	// std::cout << "seq:proc" << std::endl;

	jiss_time buffer_time = (jiss_time)nframes;

	//current_frame_in_buffer = 0;

	events_map::iterator it = events.lower_bound(current_time);
	// if (it == m->t.end()) std::cout << "end" << std::endl;
	while(true) {
		if ((it == events.end()) || (current_time_in_buffer + (it->first - current_time)) > buffer_time)  {
			if (!do_process) {
				current_time += buffer_time - current_time_in_buffer;
				e->seq_time_in_buffer = current_time_in_buffer = buffer_time;
			}
			return;
		}

		e->seq_time_in_buffer = current_time_in_buffer += it->first - current_time;
		current_time = it->first;

		//current_frame_in_buffer = current_time_in_buffer * jack_get_sample_rate(e->client);


		//! current time might be overwritten by relocate
		jiss_time t = current_time;

		//! bind the lua global variable s to this sequence
		luaL_dostring(e->lua_state, s_bind.c_str());

		do {
			// std::cout << it->first << std::endl;
			lua_event *l = dynamic_cast<lua_event*>(it->second.get());
			if (l) {
				exec_lua_event(l);
			}

			cpp_event *c = dynamic_cast<cpp_event*>(it->second.get());
			if (c) {
				exec_cpp_event(c);
			}
		} while ((++it)->first == t);
		
		// events_map::iterator new_it = events.lower_bound(current_time);
		// if (it == new_it) ++it;
		// else it = new_it;
	}
}
