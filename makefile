.PHONY: jiss.so

jiss.so:
	swig -c++ -lua -external-runtime luarun.h
	swig -c++ -lua jiss.i
	g++ -g -Wall -shared -fPIC -I /usr/include/lua5.1 -o jiss.so *.cc *.cxx -llua5.1 -ljack