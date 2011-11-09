#include <jack/jack.h>
#include <map>
#include <cstdlib>
#include <iostream>

#include <unistd.h>

#include "engine.h"

typedef std::multimap<jack_nframes_t, jack_nframes_t> event_map;


int main() {
	engine e;

	sleep(15);
}

