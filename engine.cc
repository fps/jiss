#ifdef SWIG
%module seqpp
%{
#include "engine.h"
%}
#endif

#include "engine.h"

extern "C" {
        int process(jack_nframes_t nframes, void *arg) {
                return ((engine*)arg)->process(nframes, arg);
        }
}

