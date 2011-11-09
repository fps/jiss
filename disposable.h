#ifndef DISPOSABLE_HH
#define DISPOSABLE_HH

#include <boost/shared_ptr.hpp>

#include "disposable_base.h"
#include "heap.h"

#include <iostream>

template <class T>
struct disposable : public disposable_base {
	T t;

	/**
		The only way to create a disposable<T> is by using this create() method
	*/
	static boost::shared_ptr<
		disposable<T> 
	> create(const T& t = T()) 
	{
		return boost::shared_ptr<disposable<T> > (
			heap::get()->add(
				boost::shared_ptr<disposable<T> > (new disposable<T>(t))
			)
		);
	}


	virtual ~disposable() { /* std::cout << "~disposable()" << std::endl; */ }

	private:
		/**
			Force creation on the heap by making the constructor private.
		*/
		disposable(const T &t = T()) : t(t) { 
			// std::cout << "disposable()" << std::endl;
		}
};


#endif
