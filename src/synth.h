#ifndef JISS_SYNTH_HH
#define JISS_SYNTH_HH

#include <vector>
#include <algorithm>
#include <functional>
#include <cmath>

#include "debug.h"

#include <boost/shared_ptr.hpp>
#include <jack/jack.h>

namespace jissing {

using std::vector;

typedef float float_num;
typedef unsigned int uint_num;
typedef int int_num;







template<int number>
struct n {
	const uint_num _n;

	n() : _n(number) { 
		//jdbgc("constructing")
	}

	~n() {
		//jdbgc("destroying")
	}
};

n<0> _0;
n<1> _1;
n<2> _2;
n<3> _3;





struct outs {
	vector<vector<float_num> > outs;
	virtual ~outs() { }
};







template<class nchannels>
struct op_nchannels : outs {
	op_nchannels() { 
		//jdbgc("constructing ")
	}

	nchannels n;

	uint_num nframes;
	float_num samplerate;

	virtual void init(uint_num nframes, float_num samplerate) {
		this->nframes = nframes;
		this->samplerate = samplerate;

		jdbgc("init() " << nframes)
		outs = vector<vector<float_num> > (n._n);
		for (uint_num i = 0; i < n._n; ++i) {
			outs[i] = vector<float_num>(nframes);
		}
	}

	virtual void operator()(vector<float_num> &variables) = 0;

	virtual ~op_nchannels() { 
		// jdbgc("~op_nchannels() ") 
	}
};






template<class nchannels>
struct in_op : op_nchannels<nchannels> {
	in_op() {
		jdbgc("constructing ")
	}

	// in(int n) : n(n) { }
	virtual void operator()(vector<float_num> &variables) {
		jdbgc("operator() ")
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
	vector<vector<float_num> > outs;

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








template<class Freq, class Phase>
struct sin_op : op_nchannels<n<1> >{
	Freq u1; Phase u2; 

	float_num phase;

	sin_op(Freq u1, Phase u2) 
	: 
		u1(u1), u2(u2), phase(0.0f)
	{
		jdbgc("constructing ")
	}

	void operator()(vector<float_num> &vars) {
		jdbgc("operator()")

		u1(vars);
		u2(vars);

		for (uint_num n = 0; n < nframes; ++n) {
			outs[0][n] = ::sin(phase + u2.outs[0][n]);
			phase += u1.outs[0][n] / samplerate;
		}

		jdbgc("phase: " << phase)
	}

	virtual void init(uint_num nframes, float_num samplerate) {
		op_nchannels::init(nframes, samplerate);

		u1.init(nframes, samplerate);
		u2.init(nframes, samplerate);
	}
};

template<class Freq, class Phase>
sin_op<Freq, Phase> sin(Freq u1, Phase u2) {
	return sin_op<Freq, Phase>(u1, u2);
}









struct _const : op_nchannels<n<1> > {
	_const(float_num t) : t(t) { 
		jdbgc("constructing")
	}

	const float_num t;

	virtual void init(uint_num nframes, float_num samplerate) {
		jdbgc("init()")
		op_nchannels::init(nframes, samplerate);

		std::fill(outs[0].begin(), outs[0].end(), t);
	}

	virtual void operator()(vector<float_num> &variables) {
		jdbgc("operator()")
	}
};











template<class U1, class U2>
struct mul_op : op_nchannels<n<1> > {
	U1 u1;
	U2 u2;

	mul_op(U1 u1, U2 u2) : u1(u1), u2(u2) {
		jdbgc("constructing ")
	}

	virtual void operator()(vector<float_num> &variables) {
		jdbgc("operator()")
		u1(variables);
		u2(variables);

		std::transform(
			u1.outs[0].begin(), u1.outs[0].end(), 
			u2.outs[0].begin(), 
			outs[0].begin(), 
			std::multiplies<float_num>()
		);

	}

	virtual void init(uint_num nframes, float_num samplerate) {
		jdbgc("init() ")
		op_nchannels::init(nframes, samplerate);

		u1.init(nframes, samplerate);
		u2.init(nframes, samplerate);
	}
};

template<class U1, class U2>
mul_op<U1, U2> operator*(U1 u1, U2 u2) {
	return mul_op<U1, U2>(u1, u2);
}









template<class U1, class U2>
struct add_op : op_nchannels<n<1> > {
	U1 u1;
	U2 u2;

	add_op(U1 u1, U2 u2) : u1(u1), u2(u2) {
		jdbgc("constructing ")
	}

	virtual void operator()(vector<float_num> &variables) {
		u1(variables);
		u2(variables);

		std::transform(
			u1.outs[0].begin(), u1.outs[0].end(), 
			u2.outs[0].begin(), 
			outs[0].begin(), 
			std::plus<float_num>()
		);
	}

	virtual void init(uint_num nframes, float_num samplerate) {
		jdbgc("init() ")
		op_nchannels::init(nframes, samplerate);
		u1.init(nframes, samplerate);
		u2.init(nframes, samplerate);
	}
};

template<class U1, class U2>
add_op<U1, U2> operator+(U1 u1, U2 u2) {
	return add_op<U1, U2>(u1, u2);
}






struct synth_base {
	vector<float_num> variables;

	vector<vector<float_num> > ins;
	vector<vector<float_num> > outs;

	vector<jack_port_t*> out_ports;

	synth_base(uint_num nvars, uint_num nins, uint_num nouts) 
	:
		variables(nvars),
		ins(nins),
		outs(nouts)
	{

	}

	virtual ~synth_base() { }

	virtual void operator()() = 0;

	virtual void init(uint_num nframes, float_num samplerate) {
		for (uint_num n = 0; n < ins.size(); ++n) {
			ins[n] = vector<float_num>(nframes);
		}

		for (uint_num n = 0; n < outs.size(); ++n) {
			outs[n] = vector<float_num>(nframes);
		}
	}
};

template<class nvariables, class inchannels, class outchannels, class U>
struct synth_op : synth_base {

	U u;

	synth_op(nvariables n, inchannels n_in, outchannels n_out, U u) 
	: 
		synth_base(n._n, n_in._n, n_out._n),
		u(u)
	{
		jdbgc("constructing")
	}

	virtual void operator()() {
		jdbgc("operator()")

		u(variables);

		for (uint_num channel = 0; channel < outs.size(); ++channel) {
			for (uint_num frame = 0; frame < outs[channel].size(); ++frame) {
				outs[channel][frame] = u.outs[channel][frame];
			}		
		}
	}

	virtual void init(uint_num nframes, float_num samplerate) {
		jdbgc("init()")
		synth_base::init(nframes, samplerate);
		u.init(nframes, samplerate);
	}
};

typedef boost::shared_ptr<synth_base> synth_ptr;

template<class nvariables, class inchannels, class outchannels, class U>
synth_ptr synth(
	uint_num nframes, 
	float_num samplerate, 
	nvariables n, 
	inchannels n_in, 
	outchannels n_out, 
	U u
) {
	synth_ptr p(new synth_op<nvariables, inchannels, outchannels, U>(n, n_in, n_out, u));
	p->init(nframes, samplerate);
	return p;
}


} // namespace



#endif