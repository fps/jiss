#ifndef JISS_EVENT_HH
#define JISS_EVENT_HH

#include "disposable.h"
#include "event_visitor.h"

namespace jiss {

struct event {
	bool deferred;

	event(bool deferred = false) : deferred(deferred) { }
	virtual void accept_visitor(event_visitor *visitor) {
		visitor->accept(this);
	}

	virtual ~event() { }
};

typedef boost::shared_ptr<event> event_ptr;
typedef disposable<event> disposable_event;
typedef boost::shared_ptr<disposable<event> > disposable_event_ptr;

} // namespace

#endif

