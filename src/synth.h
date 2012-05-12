#ifndef JISS_SYNTH_HH
#define JISS_SYNTH_HH

#include <vector>
#include <algorithm>
#include <functional>

namespace jiss {

typedef float num;

template<int channel>
struct in {
	std::vector<num>  out;

	// in(int n) : n(n) { }

	void operator()(num *vars) {

	}
};


//unary_op(minus, { return -u; })

template<class U, class F, int channels>
struct unary {
	std::vector<std::vector<num> > outs;

	U u;
	F f;

	// unary(U u, F f) : u(u), f(f) { }

	void operator()() {
		for (unsigned int channel; channel < channels; ++channel) {
			u();
			std::transform(u.outs[channel].begin(), u.outs[channel].end(), outs[channel].begin(), f);
		}
	}
};

struct minus1 {
	num operator()(num &t) { return -t; } 
};

template<class U, int channels> 
struct minus : unary<U, minus1, channels> {
	void operator()() {
		minus1::operator()();
	}
};



template<int channels>
struct _N {
	int n;
};


template<class U1, class U2, class U3>
struct sin {
	U1 u1; U2 u2; U3 u3;

	// sin(U1 u1, U2 u2, U3 u3) {

	// }
	void operator()(num *vars) {
		u1(vars);
		u2(vars);
		u3(vars);
	}
};

struct synth_base {
	virtual ~synth_base() { }

	virtual void operator()() = 0;
};

template<int variables, class U>
struct synth : synth_base {
	U u;

	num vars[variables];
	// synth_(U u) : u(u) { }

	synth(...) {

	}

	virtual void operator()() {
		u(vars);
	}
};

struct const_num {
	num t;
};


struct _1 {
	void operator()(num *vars) {	}	
};

struct _2 {
	void operator()(num *vars) {	}
};

} // namespace

#endif