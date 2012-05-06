#ifndef JISS_LV2_HH
#define JISS_LV2_HH

#include <string>
#include <lilv/lilv.h>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <jack/jack.h>

#include "debug.h"

namespace jiss {

struct engine;

struct lv2 {
	LilvInstance *instance;

	lv2(const std::string &uri, engine &e);

	std::vector<jack_port_t*> jack_ports;

	~lv2() {
		jdbg("~lv2()")
	}
};

typedef boost::shared_ptr<lv2> lv2_ptr;

} // namespace

#endif
