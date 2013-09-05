#ifndef NIHSTRING_H
#define NIHSTRING_H 1

#include <string>

namespace nihstring_ {

template <typename T>
class has_release {
	typedef char yes[1];
	typedef char no[2];
	template <typename U, U> struct type_check;
	
	template <typename C>
	static yes& check(type_check<C& (C::*)(), &C::release> *);
	template <typename C>
	static no& check(...);
public:
	static const bool value = (sizeof(check<T>(0)) == sizeof(yes));
};

template <bool> struct release_impl;

template <> struct release_impl<true> {
	template <typename T>
	static T& release(T &t)
	{
		return t.release();
	}
};

template <> struct release_impl<false> {
	template <typename T>
	static T& release(T &t)
	{
		return t;
	}
};

}

template <typename T>
T& release(T& t)
{
	return nihstring_::release_impl<nihstring_::has_release<T>::value>::release(t);
}

/* POD */

struct NihString {
	int count;
	size_t size;
	char *str;
	
	static struct NihString newNihString();
	static struct NihString newNihString(const char *s);
	static struct NihString newNihString(const std::string& s);

	NihString& retain();
	NihString& release();
	void free();
	const char* c_str();
	std::string std_str();

	bool empty();
};


#endif /* NIHSTRING */
