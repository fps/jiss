
.PHONY: all

all: jiss.so

LUA=lua

CXXFLAGS ?= -DJISS_NO_DEBUG -O2 -g -Wall -fPIC `pkg-config $(LUA) jack --cflags`
#CXXFLAGS ?=  -g -Wall -fPIC `pkg-config $(LUA) jack --cflags` 
LDFLAGS ?=  `pkg-config $(LUA) jack --libs` 

#LDFLAGS += -Wl,rpath=.
#CXXFLAGS += -export-dynamic

jiss.so: luarun.h jiss_wrap.o disposable.o engine.o heap.o main.o sequence.o event_visitor.o
	g++ -O3 -rdynamic -Wall -shared -fPIC $(CFLAGS) -o jiss.so *.o $(LDFLAGS) 

luarun.h:
	swig -c++ -lua -external-runtime luarun.h

jiss_wrap.cc: jiss.i *.h 
	swig -c++ -debug-classes  -o jiss_wrap.cc -lua jiss.i 

.PHONY: doc clean

doc:
	install -d ../doc/lua
	install -d ../doc/cpp
	luadoc -d ../doc/lua *.lua
	doxygen Doxyfile


clean:
	rm -rf *.o *.so jiss_wrap.cc luarun.h




INSTALL = install
install: all
	$(INSTALL) -d `pkg-config --variable=INSTALL_CMOD lua5.1`
	$(INSTALL) jiss.so `pkg-config --variable=INSTALL_CMOD lua5.1`
	$(INSTALL) jissing.lua `pkg-config --variable=INSTALL_LMOD lua5.1`


.PHONY: uninstall
uninstall:
	rm `pkg-config --variable=INSTALL_CMOD lua5.1`/jiss.so
	rm `pkg-config --variable=INSTALL_LMOD lua5.1`/jissing.lua
