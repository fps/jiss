#ifndef JISS_SEQUENCE_HH
#define JISS_SEQUENCE_HH

#include "types.h"
#include "event.h"
#include "lua_event.h"
#include "disposable.h"

#include <vector>
#include <map>
#include <algorithm>

typedef std::multimap<jiss_time, disposable_base_ptr> event_map;

struct sequence {
	enum {STOPPED, STARTED} state;

	jack_client_t *client;
	jack_port_t *port;

	event_map events;

	jiss_time current_time;
	jiss_time current_time_in_buffer;
	jack_nframes_t current_frame_in_buffer;

	void insert_lua_event(jiss_time t, const lua_event e) {
		events.insert(std::make_pair(t, disposable<lua_event>::create(e))); 
	}
	
		/**
		Precondition: current_time has to be set to the time corresponding to the 
		first frame in the buffer to process
	*/
	int process(jack_nframes_t nframes, void *arg) {

		if (state == STOPPED) return 0;

		double buffer_time = (jiss_time)nframes/(jiss_time)jack_get_sample_rate(client);

		current_frame_in_buffer = 0;
		current_time_in_buffer = 0;

		event_map::iterator it = events.lower_bound(current_time);
		// if (it == m->t.end()) std::cout << "end" << std::endl;
		while(true) {
			if ((it == events.end()) || (current_time_in_buffer + (it->first - current_time)) > buffer_time)  {
				current_time += buffer_time - current_time_in_buffer;
				current_time_in_buffer = buffer_time;
				return 0;
			}

			current_time_in_buffer += it->first - current_time;
			current_time = it->first;

			current_frame_in_buffer = current_time_in_buffer * jack_get_sample_rate(client);

			disposable<console_event>* c = dynamic_cast<disposable<console_event>*>(it->second.get());
			if (c) {
				std::cout << c->t.msg << std::flush;
			}
			
			disposable<lua_event>* l = dynamic_cast<disposable<lua_event>*>(it->second.get());
			if (l) {
				exec_lua_event(l->t.code);
			}
			event_map::iterator new_it = m->t.lower_bound(current_time);
			if (it == new_it) ++it;
			else it = new_it;
		}

		//current_time += (jiss_time)nframes/(jiss_time)jack_get_sample_rate(client);
		return 0;
	}
	
};

typedef disposable<sequence> gc_sequence;
typedef boost::shared_ptr<gc_sequence> gc_sequence_ptr;
typedef std::vector<gc_sequence_ptr> gc_sequence_ptr_vector;

#endif
