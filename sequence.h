#ifndef JISS_SEQUENCE_HH
#define JISS_SEQUENCE_HH

#include "types.h"
#include "event.h"
#include "lua_event.h"
#include "disposable.h"

#include <vector>
#include <map>
#include <algorithm>
#include <jack/jack.h>

struct engine;

typedef std::multimap<jiss_time, disposable_base_ptr> event_map;

struct sequence {
	enum {STOPPED, STARTED} state;

	event_map events;

	jiss_time current_time;
	jiss_time current_time_in_buffer;
	jack_nframes_t current_frame_in_buffer;

	void insert(jiss_time t, const lua_event e) {
		events.insert(std::make_pair(t, disposable<lua_event>::create(e))); 
	}
	
	/**
		Precondition: current_time has to be set to the time corresponding to the 
		first frame in the buffer to process
	*/
	void process(jack_nframes_t nframes, engine *e);
	
};

typedef disposable<sequence> gc_sequence;
typedef boost::shared_ptr<gc_sequence> gc_sequence_ptr;
typedef disposable<std::vector<gc_sequence_ptr> > gc_sequence_ptr_vector;
typedef boost::shared_ptr<gc_sequence_ptr_vector> gc_sequence_ptr_vector_ptr;

#endif
