#include "sequence.h"

#include "engine.h"

namespace jiss {

sequence::sequence(engine *e, const std::string &name) : 
	state(STOPPED),
	name(name),
	e(e),
	port(0),
	current_time_in_sequence(0),
	was_relocated(false)
{ 
	s_bind = "s = " + name;
	if (0 != e) {
		port = jack_port_register(e->client, name.c_str(), JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput | JackPortIsTerminal, 0);
	}
}

sequence::~sequence() {
	jdbg("~sequence()")
	jack_port_unregister(e->client, port);
}


void sequence::connect(const std::string &destinationport) {
	jack_connect(e->client, jack_port_name(port), destinationport.c_str());
}

void sequence::start_() { 
	if (state == STOPPED) {
		state = STARTED; 
	}
	state = STARTED; 
}


void sequence::start() {
	e->write_blocking_command(jiss::assign(state, STARTED));
}

void sequence::stop() {
	e->write_blocking_command(jiss::assign(state, STOPPED));
}

void sequence::exec_cpp_event(cpp_event *c) {
	c->o->f();
}

void sequence::exec_lua_event(lua_event *l) {
	// std::cout << l->code.c_str() << std::endl;
	luaL_dostring(e->lua_state, l->code.c_str());
}

void sequence::bind_sequence_var() {
	//! bind the lua global variable s to this sequence
	luaL_dostring(e->lua_state, s_bind.c_str());
}

void sequence::start(jiss_time t) {

}

void sequence::stop(jiss_time t) {

}

} // namespace