.PHONY: jiss.so

jiss.so:
	install -d doc/lua
	install -d doc/cpp
	swig -c++ -lua -external-runtime luarun.h
	swig -c++ -lua jiss.i
	luadoc -d doc/lua *.lua examples/*.lua
	g++ -O3 -rdynamic -Wall -shared -fPIC -I /opt/local/include -I /usr/include/lua5.1 -o jiss.so *.cc *.cxx -L /opt/local/lib -L /usr/local/lib -llua5.1 -ljack 
