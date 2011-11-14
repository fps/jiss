.PHONY: jiss.so

jiss.so:
	swig -c++ -lua -external-runtime luarun.h
	swig -c++ -lua jiss.i
	swig -c++ -python -external-runtime pyrun.h
	swig -c++ -python jiss.i
	g++ -export-dynamic -g -Wall -shared -fPIC -I /usr/include/lua5.1 -o jiss.so *.cc *.cxx -llua5.1 -ljack `python-config --libs --cflags`
