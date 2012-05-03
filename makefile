.PHONY: jiss.so

jiss.so:
	swig -c++ -lua -external-runtime luarun.h
	swig -c++ -lua jiss.i
	g++ -O3 -rdynamic -Wall -shared -fPIC -I /opt/local/include -I /usr/include/lua5.1 -o jiss.so *.cc *.cxx -L /opt/local/lib -L /usr/local/lib -llua5.1 -ljack 
