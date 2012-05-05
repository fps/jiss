#ifndef JISS_LV2_HH
#define JISS_LV2_HH

#include <string>
#include <lilv/lilv.h>

#include "debug.h"

namespace jiss {

struct engine;

struct lv2 {
	LilvInstance *instance;

	lv2(const std::string &uri, engine &e);

	~lv2() {
		jdbg("~lv2()")
	}
};

} // namespace

#endif
