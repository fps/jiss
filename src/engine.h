#ifndef SEQPP_E_HH
#define SEQPP_E_HH

#include <jack/jack.h>
#include <jack/midiport.h>
#include <lilv/lilv.h>

#include <map>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/variant.hpp>

extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

#include <unistd.h>

#include <sys/time.h>

#include "disposable.h"
#include "ringbuffer.h"
#include "console_event.h"
#include "lua_event.h"
#include "heap.h"
#include "sequence.h"
#include "assign.h"
#include "types.h"
#include "store.h"
#include "debug.h"

extern "C" { 
	int process(jack_nframes_t nframes, void *arg); 
}


namespace jiss {


typedef ringbuffer<boost::function<void(void)> > command_ringbuffer;


/**
	All functions that are executed in the RT context have an underscore postfix. Example: clear_().
	You can call clear() from the non-RT context and it will post the command to the engine..
*/
struct engine {
	enum {STOPPED, STARTED} state;

	jack_client_t *client;

	jack_nframes_t nframes;

	//! set by each sequence while it runs
	jiss_time seq_time_in_buffer;

	jiss_time current_time;

	gc_sequence_ptr_vector_ptr sequences;

	command_ringbuffer commands;

	ringbuffer<char> acks;

	lua_State *lua_state;

	//! This variable can be used from within cpp_events
	//! to setup/access global state.
	boost::shared_ptr<disposable<std::vector<store_base_ptr> > > storage;

	LilvWorld *lilv_world;


	engine(const std::string name = "jiss");

	~engine() {
		jdbg("~engine()")

		clear();

		heap::get()->cleanup();

		delete default_sequence;

		jdbg("we're clear")

		jack_client_close(client);
		jack_deactivate(client);
		lua_close(lua_state);
	}




	template<class T>
	void storage_assign(unsigned int index, const T &t) {
		storage->t[index] = store_base_ptr(store<T>(t));
	}

	template<class T>
	void storage_append(const T &t) {
		storage->t.push_back(store_base_ptr(new store<T>(t)));
	}


	//! Do not call these functions from outside the process
	//! callback

	//! access element index of store as T. might raise an exception when the
	//! cast fails
	template<class T> 
	T &storage_at(unsigned int index) {
		return (boost::dynamic_pointer_cast<store<T> >(storage->t[index]))->t;
	}

	//! Run a lua script in the engine global context
	void run(const std::string &code);

	void exec_lua(lua_State *state, const std::string &code);

	void exec_lua_event(lua_event *l) { run(l->code); };

	void exec_cpp_event(cpp_event *l) {
		try {
			jdbg("exec_cpp_event()")
			l->o->f();
			jdbg("done()")
		} catch (...) {

		}
	}

	//! Set to this in process(). This is only useful from within compiled cpp functions..
	//! So if you use those, do not instantiate two engine instances in the same executable
	//! (e.g. the same lua interpreter session
	static engine *e;
	static engine *get() {
		return e;
	}

	//! Will return the currently processing sequence
	sequence *default_sequence;
	sequence *s;
	sequence *current_sequence() {
		return s;
	}

	void clear() {
		gc_sequence_ptr_vector_ptr p = gc_sequence_ptr_vector::create();
		write_blocking_command(jiss::assign(sequences, p));
	}

	unsigned int num_sequences() {
		return sequences->t.size();
	}

	//! Clear vector of sequences
	void clear_() {
		sequences->t.clear();
	}

	sequence& at(unsigned int index) {
		if (index >= sequences->t.size()) { std::cout << "sequence index out of bounds" << std::endl; }

		return (sequences->t[index]->t);
	}


	//! Write a cmd and wait until completion.. Never call this from within the process thread
	//! It'll never return..
	int cmds_pending;
	void write_blocking_command(boost::function<void()> f) {
		++cmds_pending;
		commands.write(f);

		while(cmds_pending > 0) {
			usleep(1000);
		}
	}

	void register_sequence(const sequence &s);

	int process(jack_nframes_t nframes, void *arg);

	void start_() {
		//std::cout << "start" << std::endl;
		state = STARTED;
	}

	void stop_() {
		//std::cout << "stop" << std::endl;
		state = STOPPED;
	}

	jiss_time get_samplerate() {
		return jack_get_sample_rate(client);
	}


	//! These functions are safe to be executed not in the process
	//! callback

	//! Assign a new sequence to existing sequence at index
	void assign(unsigned int index, sequence &s) {
		//! copy existing sequences into a new vector
		gc_sequence_ptr_vector_ptr p = gc_sequence_ptr_vector::create(sequences->t);

		//! Assign the new sequence at the index
		p->t[index] = disposable<sequence>::create(s);

		//! commit to process thread
		register_sequence(p->t[index]->t);

		write_blocking_command(jiss::assign(sequences, p));
	}

	//! Append sequence to the vector of sequences
	void append(sequence &s) {
		//gc_sequence_ptr_vector_ptr new_seqs = gc_
		gc_sequence_ptr_vector_ptr p = gc_sequence_ptr_vector::create(sequences->t);
		gc_sequence_ptr s2 = gc_sequence::create(s);
		// std::cout << "seqsize: " << s2->t.events.size() << " "  << s2->t.state << std::endl;
		p->t.push_back(s2);
		register_sequence(s2->t);
		write_blocking_command(jiss::assign(sequences, p));
	}

	void remove(const int index) {
		gc_sequence_ptr_vector_ptr p = gc_sequence_ptr_vector::create(sequences->t);
		p->t.erase(p->t.begin() + index);
		write_blocking_command(jiss::assign(sequences, p));
	}


	void start() {
		//! turn GC off before entering STARTED state
		// lua_gc(lua_state, LUA_GCSTOP, 0);
		write_blocking_command(boost::bind(&engine::start_, this));
	}

	void stop() {
		write_blocking_command(boost::bind(&engine::stop_, this));
		// lua_gc(lua_state, LUA_GCRESTART, 0);
		//! run GC after stopping :D
	}


};

} // namespace

#endif

