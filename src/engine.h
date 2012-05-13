#ifndef SEQPP_E_HH
#define SEQPP_E_HH

#include <jack/jack.h>
#include <unistd.h>
#include <boost/function.hpp>
#include <sys/time.h>


#include <map>


#include "disposable.h"
#include "ringbuffer.h"
#include "console_event.h"
#include "lua_event.h"
#include "heap.h"
#include "assign.h"
#include "types.h"
#include "store.h"
#include "debug.h"
// #include "synth.h"

extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

extern "C" { 
	int process(jack_nframes_t nframes, void *arg); 
}

struct lua_State;

namespace jiss {


struct engine_pimpl;
struct cpp_event;
struct lua_event;
struct sequence;


/**
	All functions that are executed in the RT context have an underscore postfix. Example: clear_().
	You can call clear() from the non-RT context and it will post the command to the engine..
*/
struct engine {
	engine_pimpl *pimpl;

	engine(const std::string name = "jiss");
	~engine();	

	boost::shared_ptr<disposable<std::vector<store_base_ptr> > > storage;

	template<class T>
	void storage_assign(unsigned int index, const T &t) {
		storage->t[index] = store_base_ptr(store<T>(t));
	}

	template<class T>
	void storage_append(const T &t) {
		jdbg("appending to storage")
		boost::shared_ptr<disposable<std::vector<store_base_ptr> > > 
			new_storage(disposable<std::vector<store_base_ptr> >::create(storage->t));

		new_storage->t.push_back(store_base_ptr(new store<T>(t)));
		write_blocking_command(jiss::assign(storage, new_storage));
		jdbg("done")
	}

	template<class T> 
	T &storage_at(unsigned int index) {
		return (boost::dynamic_pointer_cast<store<T> >(storage->t[index]))->t;
	}

	//! Run a lua script in the engine global context
	void run(const std::string &code);
	void exec_lua(lua_State *state, const std::string &code);

	void exec_lua_event(lua_event *l);
	void exec_cpp_event(cpp_event *l);

	static engine *e;
	static engine *get() {
		return e;
	}

	sequence *current_sequence();

	void clear();
	unsigned int num_sequences();
	void clear_();

	sequence& at(unsigned int index);
	void register_sequence(const sequence &s);

	int process(jack_nframes_t nframes, void *arg);

	void start_();
	void stop_();

	jiss_time get_samplerate();
	jiss_time get_buffersize();


	//! These functions are safe to be executed not in the process
	//! callback

	//! Assign a new sequence to existing sequence at index
	void assign(unsigned int index, sequence &s);

	//! Append sequence to the vector of sequences
	void append(sequence &s);

	void remove(const int index);

	void start();
	void stop();

	void write_blocking_command(boost::function<void()> f);

	jack_client_t *get_client();
};

} // namespace

#endif

