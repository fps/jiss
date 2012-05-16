#ifndef JISS_EVENT_VISITOR_HH
#define JISS_EVENT_VISITOR_HH

namespace jiss {

struct event;

struct event_visitor {
	virtual void accept(event *);

	virtual ~event_visitor() { }
};

} // namespace

#endif
