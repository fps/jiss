#include "synth.h"

using namespace jiss;

int main() {
	synth_ptr s = 
		synth(
			2048, _0, _3, _1,
			sin(in(_0), _const(440.0),	in(_2)) * sin(in(_0), _const(440.0), in(_2))
		);

	(*s)();

	//jdbg(s->outs[0][0])
	std::cout << s->outs[0][0] << std::endl;
}