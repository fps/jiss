%module jiss
%{
#include "types.h"
#include "engine.h"
#include "sequence.h"
#include "lua_event.h"
%}

%include "std_string.i"
%include "std_vector.i"

%include "types.h"
%include "engine.h"
%include "sequence.h"
%include "lua_event.h"

namespace std {
%template (sequence_vector) vector<sequence>;
%template (event_vector) vector<event*>;
}
