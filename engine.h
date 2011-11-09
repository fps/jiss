#ifndef SEQPP_E_HH
#define SEQPP_E_HH

#ifdef SWIG
%module seqpp
%{
#endif

#include <jack/jack.h>
#include <map>

#include "disposable.h"

extern "C" { int process(jack_nframes_t nframes, void *arg); }

typedef std::multimap<jack_nframes_t, jack_nframes_t> event_map;
typedef boost::shared_ptr<disposable<event_map> > disposable_event_map_ptr;

struct engine {
	jack_client_t *client;
	jack_port_t *port;

	boost::shared_ptr<disposable<event_map> > m;

	int process(jack_nframes_t, void *arg) {
		event_map::iterator it = m->t.lower_bound(100000);
		if (it == m->t.end()) std::cout << "end" << std::endl;
		return 0;
	}
	
	engine() {
		m = disposable<event_map>::create(event_map());
		for (unsigned int i = 0; i < 1000000; ++i) m->t.insert(std::make_pair(i, 1));
	
		client = jack_client_open("seq++", JackNullOption, 0);
		port = jack_port_register(client, "out0", JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput | JackPortIsTerminal, 0);
		jack_set_process_callback(client, ::process, this);

		jack_activate(client);
	}

	~engine() {
		jack_client_close(client);
		jack_deactivate(client);
	}
};

#ifdef SWIG
%}
#endif


#endif

