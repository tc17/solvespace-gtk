#include "nihstring.h"
#include "solvespace.h"

struct NihString NihString::newNihString()
{
	NihString rv = {0, 0, NULL};

	return rv;
}
	
struct NihString NihString::newNihString(const char *s)
{
	NihString rv;
	
	rv.count = 1;
	rv.size = strlen(s);
	rv.str = static_cast<char *>(MemAlloc(rv.size + 1));
	strcpy(rv.str, s);

	return rv;
}

struct NihString NihString::newNihString(const std::string& s)
{
	return newNihString(s.c_str());
}

NihString& NihString::retain()
{
	count++;
	return *this;
}

NihString& NihString::release()
{	
	count--;
	if (count)
		return *this;
	
	if (count < 0)
		Error("string already released");

	free();
	return *this;
}

void NihString::free()
{
	MemFree(str);
	size = 0;
	str = NULL;	
}

const char* NihString::c_str()
{
	return str;
}

std::string NihString::std_str()
{
	return std::string(str);
}

bool NihString::empty()
{
	return (size == 0);
}
