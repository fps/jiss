#ifndef JISS_LUA_EVENT_HH
#define JISS_LUA_EVENT_HH

#include "event.h"

struct lua_event : public event {
	lua_event(const std::string &code) :
		code(code)
	{

	}
	std::string code;
};

#endif

