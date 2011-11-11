#ifndef JISS_CONSOLE_EVENT_HH
#define JISS_CONSOLE_EVENT_HH

#include "event.h"

#include <string>

struct console_event : public event {
	std::string msg;
};

#endif

