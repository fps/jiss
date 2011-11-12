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

#include <unistd.h>

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



/**
	All functions that are executed in the RT context have an underscore postfix. Example: clear_().
	You can call clear() from the non-RT context and it will post the command to the engine..
*/
struct engine {
	enum {STOPPED, STARTED} state;

	jack_client_t *client;
	jack_port_t *port;

	gc_sequence_ptr_vector_ptr sequences;

	command_ringbuffer commands;

	ringbuffer<char> acks;

	double speed;

	lua_State *lua_state;

	std::string lua_init;

	void clear() {
		write_blocking_command(boost::bind(&engine::clear, this));
	}

	//! Clear vector of sequences
	void clear_() {
		sequences->t.clear();
	}

	sequence& at(unsigned int index) {
		if (index >= sequences->t.size()) { std::cout << "sequence index out of bounds" << std::endl; }

		return (sequences->t[index]->t);
	}

	int cmds_pending;
	void write_blocking_command(boost::function<void()> f) {
		++cmds_pending;
		commands.write(f);

		while(cmds_pending > 0) {
			usleep(1000);
		}
	}

	//! Assign a new sequence to existing sequence at index
	void assign(unsigned int index, sequence &s) {
		gc_sequence_ptr_vector_ptr p = gc_sequence_ptr_vector::create(sequences->t);
		write_blocking_command(::assign(sequences, p));
	}

	//! Append sequence to the vector of sequences
	void append(sequence &s) {
		//gc_sequence_ptr_vector_ptr new_seqs = gc_
		gc_sequence_ptr_vector_ptr p = gc_sequence_ptr_vector::create(sequences->t);
		gc_sequence_ptr s2 = gc_sequence::create(s);
		std::cout << "seqsize: " << s2->t.events.size() << " "  << s2->t.state << std::endl;
		p->t.push_back(s2);
		write_blocking_command(::assign(sequences, p));
	}

	/**
		Precondition: current_time has to be set to the time corresponding to the 
		first frame in the buffer to process
	*/
	int process(jack_nframes_t nframes, void *arg) {

		while(commands.can_read()) {
			//std::cout << "." << std::endl;
			commands.read()();
			acks.write(0);
			--cmds_pending;
		}

		if (state == STOPPED) return 0;

		for (unsigned int index = 0; index < sequences->t.size(); ++index) {
			//std::cout << "|" << std::endl;
			sequences->t[index]->t.process(nframes);
		}

		return 0;
#if 0
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
#endif
	}
	


	void start() {
		write_blocking_command(boost::bind(&engine::start_, this));
	}

	void stop() {
		write_blocking_command(boost::bind(&engine::stop_, this));
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

