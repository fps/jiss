#ifndef JISS_SYNTH_HH
#define JISS_SYNTH_HH

#include <vector>
#include <algorithm>
#include <functional>

#include <boost/shared_ptr.hpp>

namespace jiss {

typedef float float_num;
typedef unsigned int uint_num;
typedef int int_num;

template<class nchannels>
struct op_nchannels {
	std::vector<std::vector<float_num> > outs;
	nchannels n;
	float_num *vars;

	virtual void init(uint_num nframes, float_num *variables) {
		vars = variables;

		outs = std::vector<std::vector<float_num> > (n._n);
		for (uint_num i = 0; i < n._n; ++i) {
			outs[i] = std::vector<float_num>(nframes);
		}
	}

	virtual ~op_nchannels() { }

	virtual void operator()(std::vector<float_num> &variables) = 0;
};

template<class nchannels>
struct in_op : op_nchannels<nchannels> {
	// in(int n) : n(n) { }
	virtual void operator()(std::vector<float_num> &variables) {
		
	}
};

template<class nchannels>
in_op<nchannels> in(nchannels n) {
	return in_op<nchannels>();
}

//unary_op(minus, { return -u; })

/*
template<class U, class F, class nchannels>
struct unary_op : op_nchannels<nchannels> {
	std::vector<std::vector<float_num> > outs;

	U u;
	F f;

	// unary(U u, F f) : u(u), f(f) { }

	void operator()() {
		for (
			unsigned int channel = 0; 
			channel < op_nchannels<nchannels>::n.n; 
			++channel
		) {
			u();
			std::transform(u.outs[channel].begin(), u.outs[channel].end(), outs[channel].begin(), f);
		}
	}
};

struct minus1 {
	float_num operator()(float_num &t) { return -t; } 
};

template<class U, class nchannels> 
struct minus_op : unary<U, minus1, nchannels> {
	void operator()() {
		minus1::operator()();
	}
};

*/

template<int number>
struct n {
	const uint_num _n;

	n() : _n(number) { }
};


template<class U1, class U2, class U3>
struct sin_op {
	U1 u1; U2 u2; U3 u3;

	sin_op(U1 u1, U2 u2, U3 u3) 
	: 
		u1(u1), u2(u2), u3(u3) 
	{

	}

	void operator()(std::vector<float_num> &vars) {
		u1(vars);
		u2(vars);
		u3(vars);
	}
};

template<class U1, class U2, class U3>
sin_op<U1, U2, U3> sin(U1 u1, U2 u2, U3 u3) {
	return sin_op<U1, U2, U3>(u1, u2, u3);
}


struct synth_base {
	virtual ~synth_base() { }

	virtual void operator()() = 0;
};

template<class nvariables, class inchannels, class outchannels, class U>
struct synth_op : synth_base {
	std::vector<float_num> variables;

	std::vector<float_num> ins;
	std::vector<float_num> outs;

	U u;

	synth_op(nvariables n, inchannels n_in, outchannels n_out, U u) 
	: 
		ins(std::vector<float_num>(n_in._n)),
		outs(std::vector<float_num>(n_out._n)),
		variables(std::vector<float_num>(n._n)),
		u(u)
	{

	}

	virtual void operator()() {
		u(variables);
	}
};

typedef boost::shared_ptr<synth_base> synth_ptr;

template<class nvariables, class inchannels, class outchannels, class U>
synth_ptr synth(nvariables n, inchannels n_in, outchannels n_out, U u) {
	return synth_ptr(new synth_op<nvariables, inchannels, outchannels, U>(n, n_in, n_out, u));
}

struct _const {
	float_num t;

	_const(float_num t) : t(t) { }
};

struct _1 {
	void operator()(float_num *vars) {	}	
};

struct _2 {
	void operator()(float_num *vars) {	}
};

n<0> _0;
n<1> _1;
n<2> _2;
n<3> _3;

} // namespace



#endif