#ifndef JISS_CPP_EVENT_HH
#define JISS_CPP_EVENT_HH

#include "event.h"

#include <string>
#include <dlfcn.h>
#include <boost/shared_ptr.hpp>

namespace jiss {

typedef void(*cpp_func)();


/**
	The cpp_event represents a loadable code of c++ code..

	The function has to be contained in a shared object file named sofile
	and has to contain at least a C-linkage function void func_name(void)
	which gets executed when the event is encountered by the engine.
*/
struct cpp_object  {

	void *fd;

	cpp_object(const std::string sofile, const std::string func_name) {
		// std::cout << sofile << " : " << func_name << std::endl;
		fd = dlopen(sofile.c_str(), RTLD_NOW);
		if (fd == 0) std::cout << "dlopen failed: " << dlerror() << std::endl;

		f = (cpp_func) ((void*)dlsym(fd, func_name.c_str()));
		if (f == 0) std::cout << "dlsym failed: " << dlerror() << std::endl;
	}

	~cpp_object() {
		dlclose(fd);
	}

	cpp_func f;
};

struct cpp_event : public event {
	boost::shared_ptr<cpp_object> o;

	cpp_event(const std::string sofile, const std::string func_name) :
		o(new cpp_object(sofile, func_name))
	{

	}
};

} // namespace

#endif
