#ifndef HEAP_HH
#define HEAP_HH

#include <list>
#include <iostream>

#include "disposable_base.h"

struct heap {
	std::list<boost::shared_ptr<disposable_base> > disposables;

	static heap* instance;

	static heap* get() {
		if (instance) return instance;
		return (instance = new heap());
	}

	template <class T>
	T add(T d) {
		disposables.push_back(d);
		return d;
	}

	/**
		Note that this function has to be called in the same thread that writes commands, otherwise
		references might go away between the construction of a disposable and binding it to a functor
		that uses it.
	*/
	void cleanup() {
		for (std::list<boost::shared_ptr<disposable_base> >::iterator it = disposables.begin(); it != disposables.end();) {
			if (it->unique()) {
				it = disposables.erase(it);
			} else {
				++it;
			}
		}	
	}

	~heap() { instance = 0; }

	protected:
		heap() { }
};


#endif
