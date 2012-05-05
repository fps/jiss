
CFLAGS ?= -fPIC `pkg-config lua5.1 jack --cflags`
CXXFLAGS ?= ${CFLAGS}
LDFLAGS ?=  `pkg-config lua5.1 jack --libs`

jiss.so: luarun.h jiss_wrap.o disposable.o engine.o heap.o main.o sequence.o
	g++ -O3 -rdynamic -Wall -shared -fPIC $(CFLAGS) -o jiss.so *.o $(LDFLAGS) 

luarun.h:
	swig -c++ -lua -external-runtime luarun.h

jiss_wrap.cc: jiss.i *.h 
	swig -c++ -debug-classes  -o jiss_wrap.cc -lua jiss.i 

.PHONY: doc clean

doc:
	install -d doc/lua
	install -d doc/cpp
	luadoc -d doc/lua *.lua 


clean:
	rm -rf *.o *.so jiss_wrap.cc luarun.h