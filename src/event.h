#ifndef JISS_EVENT_HH
#define JISS_EVENT_HH

#include "disposable.h"

namespace jiss {

struct event {
	virtual ~event() { }
};

typedef boost::shared_ptr<event> event_ptr;
typedef disposable<event> disposable_event;
typedef boost::shared_ptr<disposable<event> > disposable_event_ptr;

} // namespace

#endif

