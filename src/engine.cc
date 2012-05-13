#include "engine.h"
#include "luarun.h"
#include "debug.h"
#include "sequence.h"


#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/variant.hpp>


extern "C" {
        int process(jack_nframes_t nframes, void *arg) {
                return ((jiss::engine*)arg)->process(nframes, arg);
        }
}


namespace jiss {

typedef ringbuffer<boost::function<void(void)> > command_ringbuffer;

struct engine_pimpl {
	gc_sequence_ptr_vector_ptr sequences;
	command_ringbuffer commands;
	ringbuffer<char> acks;
	int cmds_pending;

	enum {STOPPED, STARTED} state;

	jack_client_t *client;
	jack_nframes_t nframes;

	//! set by each sequence while it runs
	jiss_time seq_time_in_buffer;
	jiss_time current_time;

	lua_State *lua_state;

	sequence *default_sequence;
	sequence *s;

	engine *e;

	engine_pimpl(const std::string name, engine* e) :
		sequences(gc_sequence_ptr_vector::create(std::vector<gc_sequence_ptr>())),
		commands(1024),
		cmds_pending(0),
		state(STOPPED),
		e(e)
	{
		lua_state = luaL_newstate();
		luaL_openlibs(lua_state);
		luaL_dostring(lua_state, "require \"jiss\"");
		luaL_dostring(lua_state, "require \"jissing\"");
	
		//! Every lua script will have a variable called e in it pointing to the engine object :D
		SWIG_NewPointerObj(lua_state, this, SWIG_TypeQuery(lua_state, "jiss::engine*"), 0);
		lua_setglobal(lua_state, "e");
	
		jdbg("creating jack client")
		client = jack_client_open(name.c_str(), JackNullOption, 0);
		jack_set_process_callback(client, ::process, e);
		jack_activate(client);
		// jack_cycle_wait(client);
	
	}

	~engine_pimpl() {
		heap::get()->cleanup();
		delete default_sequence;
	
		jdbg("we're clear")
	
		jack_client_close(client);
		jack_deactivate(client);
		lua_close(lua_state);
	}

	void write_blocking_command(boost::function<void()> f) {
		++cmds_pending;
		commands.write(f);

		while(cmds_pending > 0) {
			usleep(1000);
		}
	}
};

engine *engine::e = 0;

engine::engine(const std::string name) :
	pimpl(new engine_pimpl(name, this))

{
	e = this;

	jdbg("engine()")
	//! create the storage for cpp code
	storage = disposable<std::vector<boost::shared_ptr<store_base> > >::create(std::vector<boost::shared_ptr<store_base> >());
	
	pimpl->default_sequence = new sequence(e);
	pimpl->s  = pimpl->default_sequence;

	jdbg("setup lua")
}


engine::~engine() {
	jdbg("~engine()")
	clear();
	delete pimpl;
}


void engine::exec_lua(lua_State *state, const std::string &code) {
	luaL_dostring(state, code.c_str());
}

void engine::register_sequence(const sequence &s) {
	//! And also named like the sequence
	SWIG_NewPointerObj(pimpl->lua_state, &s, SWIG_TypeQuery(pimpl->lua_state, "sequence*"), 0);
	lua_setglobal(pimpl->lua_state, s.name.c_str());
}


void engine::run(const std::string &code) {
	//write_blocking_command(boost::bind(exec_lua, lua_state, code.c_str()));
	exec_lua(pimpl->lua_state, code);
}


/**
	Precondition: current_time has to be set to the time corresponding to the 
	first frame in the buffer to process
*/
int engine::process(jack_nframes_t frames, void *arg) {
	e = this;
	pimpl->nframes = frames;

	while(pimpl->commands.can_read()) {
		//std::cout << "." << std::endl;
		pimpl->commands.read()();
		pimpl->acks.write(0);
		--(pimpl->cmds_pending);
	}
	//! clear the sequences midi buffer even if we are not running
	for (unsigned int index = 0; index < pimpl->sequences->t.size(); ++index) {
		jack_midi_clear_buffer(jack_port_get_buffer(pimpl->sequences->t[index]->t.port, frames));
	}

	if (pimpl->state == engine_pimpl::STOPPED) return 0;
	for (unsigned int frame = 0; frame < pimpl->nframes; ++frame) {
		for (unsigned int index = 0; index < pimpl->sequences->t.size(); ++index) {
			pimpl->s = &(pimpl->sequences->t[index]->t);
			pimpl->s->current_frame_in_buffer = frame;
			pimpl->sequences->t[index]->t.process();
		}
	} 
	return 0;
}

void engine::write_blocking_command(boost::function<void()> f) {
	pimpl->write_blocking_command(f);
}

void engine::start() {
	//! turn GC off before entering STARTED state
	// lua_gc(lua_state, LUA_GCSTOP, 0);
	write_blocking_command(boost::bind(&engine::start_, this));
}

void engine::stop() {
	write_blocking_command(boost::bind(&engine::stop_, this));
	// lua_gc(lua_state, LUA_GCRESTART, 0);
	//! run GC after stopping :D
}


sequence *engine::current_sequence() {
	return pimpl->s;
}

void engine::clear() {
	gc_sequence_ptr_vector_ptr p = gc_sequence_ptr_vector::create();
	write_blocking_command(jiss::assign(pimpl->sequences, p));
}

unsigned int engine::num_sequences() {
	return pimpl->sequences->t.size();
}

void engine::clear_() {
	pimpl->sequences->t.clear();
}

sequence& engine::at(unsigned int index) {
	if (index >= pimpl->sequences->t.size()) { std::cout << "sequence index out of bounds" << std::endl; }

	return (pimpl->sequences->t[index]->t);
}

void engine::start_() {
	pimpl->state = engine_pimpl::STARTED;
}

void engine::stop_() {
	pimpl->state = engine_pimpl::STOPPED;
}

jiss_time engine::get_samplerate() {
	return jack_get_sample_rate(pimpl->client);
}

jiss_time engine::get_buffersize() {
	return jack_get_buffer_size(pimpl->client);
}

//! Assign a new sequence to existing sequence at index
void engine::assign(unsigned int index, sequence &s) {
	gc_sequence_ptr_vector_ptr p = gc_sequence_ptr_vector::create(pimpl->sequences->t);
	p->t[index] = disposable<sequence>::create(s);
	register_sequence(p->t[index]->t);
	write_blocking_command(jiss::assign(pimpl->sequences, p));
}

//! Append sequence to the vector of sequences
void engine::append(sequence &s) {
	//gc_sequence_ptr_vector_ptr new_seqs = gc_
	gc_sequence_ptr_vector_ptr p = gc_sequence_ptr_vector::create(pimpl->sequences->t);
	gc_sequence_ptr s2 = gc_sequence::create(s);
	// std::cout << "seqsize: " << s2->t.events.size() << " "  << s2->t.state << std::endl;
	p->t.push_back(s2);
	// register_sequence(s2->t);
	write_blocking_command(jiss::assign(pimpl->sequences, p));
}

void engine::remove(const int index) {
	gc_sequence_ptr_vector_ptr p = gc_sequence_ptr_vector::create(pimpl->sequences->t);
	p->t.erase(p->t.begin() + index);
	write_blocking_command(jiss::assign(pimpl->sequences, p));
}

void engine::exec_lua_event(lua_event *l) { 
	run(l->code); 
};

void engine::exec_cpp_event(cpp_event *l) {
	try {
		l->o->f();
	} catch (...) {
		jdbg("some exception occured")
	}
}

jack_client_t *engine::get_client() {
	return pimpl->client;
}

} // namespace
