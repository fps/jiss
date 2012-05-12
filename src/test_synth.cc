#include "synth.h"

using namespace jiss;

int main() {

	synth_ptr s = 
		synth(
			1000, 10000.0, _0, _0, _1,
			sin(_const(440.0), _const(M_PI)) * sin(_const(444.0), _const(M_PI))
		);

	synth_ptr s2 = 
		synth(
			2048, 48000.0, _0, _0, _1,
			_const(0.3) + _const(1.2)
		);



	(*s2)();

	(*s)();
	for (int i = 0; i < 1000; ++i) {

		std::cout << s->outs[0][i] << std::endl;


		// std::cout << s2->outs[0][i] << std::endl;
	}
}