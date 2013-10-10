/*
 * this string class is intended to use with POD types
 * allocated by MemAlloc
 *
 * element with entry_ == 0 is special Nil element
 * Nil.c_str() == NULL
 * Nil.std_str().empty() == true
 * reference counting for Nil is ignored
 */

#ifndef CACHESTRING_H
#define CACHESTRING_H 1

#include <string>

/* POD */
struct CacheString {
	typedef size_t CSEntry;
	CSEntry entry_;

	static CacheString newCacheString(const char *s);
	static CacheString newCacheString(const std::string& s);
	CacheString& ref();
	CacheString& unref();
	const char* c_str();
	std::string std_str();
	bool empty();
};

#endif /* CACHESTRING */
