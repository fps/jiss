#ifndef SEQPP_E_HH
#define SEQPP_E_HH

#include <jack/jack.h>
#include <jack/midiport.h>

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
#include "sequence.h"
#include "assign.h"
#include "types.h"

extern "C" { 
	int process(jack_nframes_t nframes, void *arg); 
}



typedef std::multimap<jiss_time, disposable_base_ptr> event_map;

typedef boost::shared_ptr<disposable<event_map> > disposable_event_map_ptr;
typedef ringbuffer<boost::function<void(void)> > command_ringbuffer;



struct engine {
	enum {STOPPED, STARTED} state;

	jack_client_t *client;
	jack_port_t *port;

	gc_sequence_ptr_vector_ptr sequences;

	boost::shared_ptr<disposable<event_map> > m;

	command_ringbuffer commands;

	double speed;

	jiss_time current_time;
	jiss_time current_time_in_buffer;
	jack_nframes_t current_frame_in_buffer;

	lua_State *lua_state;

	std::string lua_init;

	//! This function must only be called when state == STOPPED
	void exec_lua_init();

	void exec_lua_event(const std::string &code);

	/**
		Precondition: current_time has to be set to the time corresponding to the 
		first frame in the buffer to process
	*/
	int process(jack_nframes_t nframes, void *arg) {
		jack_midi_clear_buffer(jack_port_get_buffer(port, 1024));
		while(commands.can_read()) commands.read()();

		if (state == STOPPED) return 0;

		double buffer_time = (jiss_time)nframes/(jiss_time)jack_get_sample_rate(client);

		current_frame_in_buffer = 0;
		current_time_in_buffer = 0;

		event_map::iterator it = m->t.lower_bound(current_time);
		// if (it == m->t.end()) std::cout << "end" << std::endl;
		while(true) {
			if ((it == m->t.end()) || (current_time_in_buffer + (it->first - current_time)) > buffer_time)  {
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
	

	//! Call from process() only
	//! Precondition: current_frame_in_buffer has to be set correctly
	void midi_note_on(unsigned int channel, unsigned int note, unsigned int velocity) {
		jack_midi_data_t data[3] = {0, 0, 0};
		data[0] |= 0x90;
		data[1] = note;
		data[2] = velocity;
		
		jack_midi_event_write(jack_port_get_buffer(port, 1024), current_frame_in_buffer, data, 3);
	}

	//! Call from process() only
	void midi_note_off(unsigned int channel, unsigned int note) {

	}


	void start() {
		commands.write(boost::bind(&engine::start_, this));
	}

	void stop() {
		commands.write(boost::bind(&engine::stop_, this));
	}

	void relocate(jiss_time t) {
		current_time = t;
	}

	engine();

	~engine() {
		jack_client_close(client);
		jack_deactivate(client);
		lua_close(lua_state);
	}


	//! Call only from process thread
	void start_() {
		//std::cout << "start" << std::endl;
		state = STARTED;

	}

	//! Call only from process thread
	void stop_() {
		//std::cout << "stop" << std::endl;
		state = STOPPED;
	}

	void set_sequences(std::vector<sequence>& seqs) {
		//commands.write(assign(
	}
};



#endif

