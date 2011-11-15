#ifndef JISS_STORE_HH
#define JISS_STORE_HH

//! The base type for all objects to be stored in the engine::store vector
struct store_base {
	virtual ~store_base() { }	
};

typedef boost::shared_ptr<store_base> store_base_ptr;

template <class T>
struct store : public store_base {
	store(const T &t = T()) : t(t) { }
	T t;
};

#endif
