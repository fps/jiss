#ifndef JISS_LUA_EVENT_HH
#define JISS_LUA_EVENT_HH

#include "event.h"

struct lua_event : public event {
	std::string code;
};

#endif

