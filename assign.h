#ifndef ASSIGN_HH
#define ASSIGN_HH

template<class U, class T>
struct assign_fun {
	U &u;
	T t;

	assign_fun(U &u, const T& t) : u(u), t(t) { }

	void operator()() { u = t; }
};

template<class U, class T>
assign_fun<U,T> 
assign(U &u,  const T& t) {
	return assign_fun<U, T>(u, t);
}

#endif
