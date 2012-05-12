#include "synth.h"

using namespace jiss;

int main() {
	synth_ptr s = 
		synth(
			_2, _3, _1,
			sin(
				in(_0),
				in(_1),
				in(_2)
			)
		);

	(*s)();
}