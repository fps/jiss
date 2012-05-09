#ifndef DISPOSABLE_BASE_HH
#define DISPOSABLE_BASE_HH

#include <boost/shared_ptr.hpp>

namespace jiss {

struct disposable_base {
	virtual ~disposable_base() { }
};

typedef boost::shared_ptr<disposable_base> disposable_base_ptr;

} // namespace

#endif
