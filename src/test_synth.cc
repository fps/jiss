#include "synth.h"

using namespace jiss;

int main() {
	synth_base *s = new
	synth<
		2,
		sin<
			_1, 
			_2, 
			in<2> 
		> 
	>;
	(*s)();

}