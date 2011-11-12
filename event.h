#ifndef JISS_EVENT_HH
#define JISS_EVENT_HH

struct event {
	virtual ~event() { }
};

typedef disposable<event> disposable_event;
typedef boost::shared_ptr<disposable<event> > disposable_event_ptr;



#endif

