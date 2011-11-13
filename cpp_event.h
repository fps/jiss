#ifndef JISS_CPP_EVENT_HH
#define JISS_CPP_EVENT_HH

#include "event.h"

#include <string>
#include <dlfcn.h>

typedef void(*cpp_func)();

/**
	The cpp_event represents a loadable code of c++ code..

	The function has to be contained in a shared object file named sofile
	and has to contain at least a C-linkage function void func_name(void)
	which gets executed when the event is encountered by the engine.
*/
struct cpp_event : public event {

	void *fd;

	cpp_event(const std::string &sofile, const std::string &func_name) {
		fd = dlopen(sofile.c_str(), RTLD_NOW);
		f = (cpp_func) ((void*)dlsym(fd, func_name.c_str()));
	}

	~cpp_event() {
		dlclose(fd);
	}

	cpp_func f;
};

#endif
