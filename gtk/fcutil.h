#ifndef FCUTIL_H
#define FCUTIL_H 1

#include <list>
#include <string>
#include <fontconfig/fontconfig.h>

class FcUtil {
	std::list<std::string> fonts_;
public:
	static FcUtil& getInstance();
	const std::list<std::string>& fonts();
	~FcUtil();
private:
	FcUtil();
	FcUtil(const FcUtil&);
	FcUtil& operator=(const FcUtil&);
};

#endif /* FCUTIL_H */
