#ifndef JISS_DEBUG_HH
#define JISS_DEBUG_HH

#include <iostream>

#ifndef JISS_NO_DEBUG
#define jdbg(x) { std::cout << x << std::endl; }
#endif

#ifdef JISS_NO_DEBUG
#define jdbg(x) { }
#endif

#ifdef __GNUC__
#undef JISS_UNMANGLE
#include <cxxabi.h>
#include <string>

inline std::string gcc_unmangle(const char *typen) {
	int status;
	char *tmp = abi::__cxa_demangle(typen, 0, 0, &status);
	std::string ret(tmp);
	free(tmp);
	return ret;
}

#define JISS_UNMANGLE(X) (gcc_unmangle(typeid(X).name()))
#else
#define JISS_UNMANGLE(X) (typeid(X).name())
#endif

#ifndef JISS_NO_DEBUG
#define jdbg(x) { std::cout << x << std::endl; }
#define jdbgc(x) { std::cout << x << " " << JISS_UNMANGLE(this) << std::endl; }
#endif

#ifdef JISS_NO_DEBUG
#define jdbg(x) { }
#endif

#endif
