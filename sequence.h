#ifndef JISS_SEQUENCE_HH
#define JISS_SEQUENCE_HH

#include "types.h"
#include "event.h"
#include "lua_event.h"
#include "disposable.h"
#include "assign.h"

#include <vector>
#include <map>
#include <algorithm>
#include <jack/jack.h>

struct engine;

typedef std::multimap<jiss_time, event_ptr> events_map;

struct sequence {
	enum {STOPPED, STARTED};
	int state;
	
	engine *e;

	events_map events;

	jiss_time current_time;
	jiss_time current_time_in_buffer;
	jack_nframes_t current_frame_in_buffer;

	void clear() {
		events.clear();
	}

	void insert(jiss_time t, const lua_event e) {
		events.insert(std::make_pair(t, event_ptr(new lua_event(e)))); 
	}

	void start();

	void relocate(jiss_time t) {
		current_time = t;
	}

	/**
		Precondition: current_time has to be set to the time corresponding to the 
		first frame in the buffer to process
	*/
	void process(jack_nframes_t nframes);

	sequence(engine *e = 0) : 
		state(STOPPED),
		e(e),
		current_time(0)
	{ 
	}
};

typedef disposable<sequence> gc_sequence;
typedef boost::shared_ptr<gc_sequence> gc_sequence_ptr;
typedef disposable<std::vector<gc_sequence_ptr> > gc_sequence_ptr_vector;
typedef boost::shared_ptr<gc_sequence_ptr_vector> gc_sequence_ptr_vector_ptr;

#endif
