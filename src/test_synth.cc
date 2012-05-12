#include "synth.h"

using namespace jiss;

int main() {
	synth_ptr s = 
		synth(
			2048, 48000.0, _0, _3, _1,
			sin(_const(440.0),	_const(M_PI)) * sin(_const(440.0), _const(0))
		);

	(*s)();

	//jdbg(s->outs[0][0])
	std::cout << s->outs[0][0] << std::endl;

	synth_ptr s2 = 
		synth(
			2048, 48000.0, _0, _3, _1,
			_const(1.2)
		);

	(*s)();

	//jdbg(s->outs[0][0])
	std::cout << s2->outs[0][0] << std::endl;
}