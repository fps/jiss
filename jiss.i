%module jiss
%{
#include "types.h"
#include "engine.h"
#include "sequence.h"
#include "event.h"
#include "lua_event.h"
#include "disposable_base.h"
#include "disposable.h"
#include <boost/shared_ptr.hpp>
%}

%include "std_string.i"
%include "std_vector.i"

%include "types.h"
%include "engine.h"
%include "sequence.h"
%include "event.h"
%include "lua_event.h"
%include "disposable_base.h"
%include "disposable.h"


namespace std {
%template (sequence_vector) vector<sequence>;
%template (event_vector) vector<event*>;
}

%template (gc_sequence) disposable<sequence>;

