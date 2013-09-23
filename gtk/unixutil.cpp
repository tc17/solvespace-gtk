#include "solvespace.h"
#include <set>
#include <map>

static std::set<void *> pool;
static std::map<void *, size_t> mem;

void *AllocTemporary(size_t n)
{
	void *p = calloc(1, n);
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

void *MemRealloc(void *p, size_t n)
{
	void *newp = realloc(p, n);
	if (!newp)
		oops();
	
	size_t oldsize = mem[p];
	if (n > oldsize)
		memset((char *)newp + oldsize, 0, n - oldsize);

	if (newp != p)
		mem.erase(p);
	mem[newp] = n;

	return newp;
}

void *MemAlloc(size_t n)
{
	void *p = calloc(1, n);
	if (!p)
		oops();

	mem[p] = n;
	return p;
}

void MemFree(void *p)
{
	if (p)
		mem.erase(p);
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
	std::string rv;
	char *rpath = realpath(path.c_str(), NULL);
	if (rpath) {
		rv = rpath;
		free(rpath);
	}

	return rv;
}

SDWORD GetMilliseconds(void)
{
}
