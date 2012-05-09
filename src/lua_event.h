#ifndef JISS_LUA_EVENT_HH
#define JISS_LUA_EVENT_HH

#include "event.h"
#include <vector>

namespace jiss {

struct lua_event : public event {
	lua_event(const std::string &code) :
		code(code)
	{

	}
	std::string code;
};

struct lua_dump_event : public event {
	lua_dump_event(const std::vector<char> chunk) :
		chunk(chunk)
	{

	}
	std::vector<char> chunk;
};

} // namespace

#endif

