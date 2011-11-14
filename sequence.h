#ifndef JISS_SEQUENCE_HH
#define JISS_SEQUENCE_HH

#include "types.h"
#include "event.h"
#include "lua_event.h"
#include "cpp_event.h"
#include "disposable.h"
#include "assign.h"

#include <vector>
#include <map>
#include <algorithm>
#include <jack/jack.h>
#include <jack/midiport.h>

struct engine;

typedef std::multimap<jiss_time, event_ptr> events_map;

struct sequence {
	enum {STOPPED, STARTED};
	int state;

	std::string name;
	
	engine *e;

	jack_port_t *port;

	//! The amount of frames to process in proces()
	jack_nframes_t nframes;

	//! Do not call in RT context
	void connect(const std::string &destinationport);

	events_map events;

	jiss_time current_time;
	jiss_time current_time_in_buffer;
	jack_nframes_t current_frame_in_buffer;


	void midi_cc(unsigned int channel, unsigned int cc, unsigned int val) {
		jack_midi_data_t data[3] = {0, 0, 0};
		data[0] |= 0xb0;
		data[0] |= channel;
		data[1] = cc;
		data[2] = val;
		
		jack_midi_event_write(jack_port_get_buffer(port, nframes), current_frame_in_buffer, data, 3);
	}

	//! Call from process() only
	//! Precondition: current_frame_in_buffer has to be set correctly
	void midi_note_on(unsigned int channel, unsigned int note, unsigned int velocity) {
		jack_midi_data_t data[3] = {0, 0, 0};
		data[0] |= 0x90;
		data[0] |= channel;
		data[1] = note;
		data[2] = velocity;
		
		jack_midi_event_write(jack_port_get_buffer(port, nframes), current_frame_in_buffer, data, 3);
	}

	//! Call from process() only
	void midi_note_off(unsigned int channel, unsigned int note) {

	}

	void clear() {
		events.clear();
	}

	void insert_cpp_event(jiss_time t, const cpp_event e) {
		events.insert(std::make_pair(t, event_ptr(new cpp_event(e)))); 
	}

	void insert(jiss_time t, const lua_event e) {
		events.insert(std::make_pair(t, event_ptr(new lua_event(e)))); 
	}

	void start();
	void stop();

	void start_();

	void stop_() { state = STOPPED; }

	void relocate(jiss_time t) {
		current_time = t;
	}

	void exec_lua_event(lua_event *e);
	//void exec_cpp_event(cpp_event *e);

	/**
		Precondition: current_time has to be set to the time corresponding to the 
		first frame in the buffer to process
	*/
	void process(jack_nframes_t nframes);

	sequence(engine *e = 0, const std::string &name = "");

	~sequence();
};

typedef disposable<sequence> gc_sequence;
typedef boost::shared_ptr<gc_sequence> gc_sequence_ptr;
typedef disposable<std::vector<gc_sequence_ptr> > gc_sequence_ptr_vector;
typedef boost::shared_ptr<gc_sequence_ptr_vector> gc_sequence_ptr_vector_ptr;

#endif
