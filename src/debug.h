#ifndef JISS_DEBUG_HH
#define JISS_DEBUG_HH

#include <iostream>

#ifndef JISS_NO_DEBUG
#define jdbg(x) { std::cout << x << std::endl; }
#endif

#ifdef JISS_NO_DEBUG
#define jdbg(x) { }
#endif

#endif
