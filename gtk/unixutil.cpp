#include "solvespace.h"
#include <set>

static std::set<void *> pool;

void *AllocTemporary(int n)
{
	void *p = malloc(n);
	if (!p)
		oops();

	pool.insert(p);
	return p;
}

void FreeTemporary(void *p) 
{
	pool.erase(p);
	free(p);
}

void FreeAllTemporary(void)
{
	std::set<void *>::const_iterator it;
	for (it = pool.begin(); it != pool.end(); ++it)
		free(*it);

	pool.clear();
}

void *MemRealloc(void *p, int n)
{
	p = realloc(p, n);
	if (!p)
		oops();
	return p;
}

void *MemAlloc(int n)
{
	void *p = malloc(n);
	if (!p)
		oops();
	return p;
}

void MemFree(void *p)
{
	free(p);
}

void dbp(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
}

std::string GetAbsoluteFilename(const std::string& path)
{
}

SDWORD GetMilliseconds(void){
}
