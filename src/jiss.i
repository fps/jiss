%module jiss
%{
	#include "types.h"
	#include "engine.h"
	#include "sequence.h"
	#include "event.h"
	#include "event_visitor.h"
	#include "lua_event.h"
	#include "cpp_event.h"
	#include "disposable_base.h"
	#include "disposable.h"
	#include "heap.h"
	#include <boost/shared_ptr.hpp>

	using namespace jiss;
%}

%include "std_string.i"
%include "std_vector.i"

%include "types.h"
%include "event_visitor.h"
%include "engine.h"
%include "sequence.h"
%include "event.h"
%include "lua_event.h"
%include "cpp_event.h"
%include "disposable_base.h"
%include "disposable.h"
%include "heap.h"

using namespace jiss;

%apply unsigned long long { jiss_time }
%apply unsigned long long { jack_nframes_t }

namespace std {
  %template (sequence_vector) vector<jiss::sequence>;
  %template (event_vector) vector<jiss::event*>;
}

%template (gc_sequence) jiss::disposable<jiss::sequence>;
 
