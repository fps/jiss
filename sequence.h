#ifndef JISS_SEQUENCE_HH
#define JISS_SEQUENCE_HH

#include "types.h"
#include "event.h"
#include "lua_event.h"
#include "disposable.h"

#include <vector>
#include <map>
#include <algorithm>

struct sequence {
	enum {STOPPED, STARTED} state;

	jack_client_t *client;
	jack_port_t *port;

	std::multimap<jiss_time, disposable_base_ptr> events;

	void insert_lua_event(jiss_time t, const lua_event e) {
		events.insert(std::make_pair(t, disposable<lua_event>::create(e))); 
	}
};

typedef disposable<sequence> gc_sequence;
typedef boost::shared_ptr<gc_sequence> gc_sequence_ptr;
typedef std::vector<gc_sequence_ptr> gc_sequence_ptr_vector;

#endif
