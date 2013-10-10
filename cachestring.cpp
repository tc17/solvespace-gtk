#include <cassert>
#include <string>
#include <queue>
#include "solvespace.h"
#include "cachestring.h"

/* POD */
struct CSCacheEntry {
	char *str_;
	int count_;
	size_t size_;
	size_t index_;

	static CSCacheEntry newCSCacheEntry(size_t index);
	CSCacheEntry& set(const char *s);
	CSCacheEntry& set(const std::string& s);
	const char* get();
	void free();
	void ref();
	bool unref();
	size_t size();
};

class CSCache {
	static const size_t preallocated = 32;
	size_t allocated_;
	CSCacheEntry *cache_;
	std::queue<CSCacheEntry*> empty_bucket_;
public:
	static const CacheString::CSEntry Nil = 0;

	static CSCache& getInstance()
	{
		static CSCache instance;
		return instance;
	}

	CacheString::CSEntry add(const char* s);
	CacheString::CSEntry add(const std::string& s);
	const char* get(CacheString::CSEntry entry);
	CacheString::CSEntry ref(CacheString::CSEntry entry);
	CacheString::CSEntry unref(CacheString::CSEntry entry);
	size_t size(CacheString::CSEntry entry);

private:
	CSCache() : allocated_(preallocated), 
			cache_(static_cast<CSCacheEntry*>(MemAlloc(sizeof(*cache_) * preallocated))),
			empty_bucket_()
	{
		populateBucket(0, allocated_);
		add(NULL); // cache_[0] special Nil entry
	}
	
	~CSCache()
	{
		for (size_t i = 0; i < allocated_; i++)
			cache_[i].free();
		MemFree(cache_);
	}
	
	CacheString::CSEntry getEmpty();

	CSCache(const CSCache&);
	CSCache& operator=(const CSCache&);

	void populateBucket(size_t start, size_t end)
	{
		for (size_t i = start; i < end; i++) {
			cache_[i] = CSCacheEntry::newCSCacheEntry(i);
			empty_bucket_.push(&cache_[i]);
		}
	}
};
	
CacheString CacheString::newCacheString(const char *s)
{
	CacheString rv;
	rv.entry_ = CSCache::getInstance().add(s);
	return rv;
}

CacheString CacheString::newCacheString(const std::string& s)
{
	CacheString rv;
	rv.entry_ = CSCache::getInstance().add(s);
	return rv;
}

CacheString& CacheString::ref()
{
	CSCache::getInstance().ref(entry_);
	return *this;
}

CacheString& CacheString::unref()
{
	CSCache::getInstance().unref(entry_);
	return *this;
}

const char* CacheString::c_str()
{
	return CSCache::getInstance().get(entry_);
}

std::string CacheString::std_str()
{
	const char *str = CSCache::getInstance().get(entry_);
	return str ? std::string(str) : std::string();
}

bool CacheString::empty()
{
	return CSCache::getInstance().size(entry_) == 0;
}

CSCacheEntry CSCacheEntry::newCSCacheEntry(size_t index)
{
	CSCacheEntry rv = { NULL, 0, 0, index };
	return rv;
}

CSCacheEntry& CSCacheEntry::set(const char *s)
{
	assert(count_ == 0);
	assert(size_ == 0);
	assert(str_ == NULL);

	count_ = 1;
	if (s) {
		size_ = strlen(s);
		str_ = static_cast<char *>(MemAlloc(size_ + 1));
		strcpy(str_, s);
	}

	return *this;
}

CSCacheEntry& CSCacheEntry::set(const std::string& s)
{
	assert(count_ == 0);
	assert(size_ == 0);
	assert(str_ == NULL);

	count_ = 1;
	if (!s.empty()) {
		size_ = s.size();
		str_ = static_cast<char *>(MemAlloc(size_ + 1));
		strcpy(str_, s.c_str());
	}

	return *this;
}

const char* CSCacheEntry::get()
{
	return str_;
}

void CSCacheEntry::free()
{
	MemFree(str_);
	size_ = 0;
	str_ = NULL;
}

void CSCacheEntry::ref()
{
	assert(count_ > 0);
	count_++;
}

bool CSCacheEntry::unref()
{
	assert(count_ > 0);
	count_--;
	if (count_ == 0) {
		free();
		return true;
	}

	return false;
}

size_t CSCacheEntry::size()
{
	return size_;
}

CacheString::CSEntry CSCache::getEmpty()
{
	if (empty_bucket_.empty()) {
		size_t new_allocated = allocated_ * 2;
		cache_ = static_cast<CSCacheEntry*>(MemRealloc(cache_, new_allocated * sizeof(*cache_)));
		populateBucket(allocated_, new_allocated);
		allocated_ = new_allocated;
	}

	CacheString::CSEntry rv = empty_bucket_.front()->index_;
	empty_bucket_.pop();

	return rv;
}

CacheString::CSEntry CSCache::add(const char* s)
{
	CacheString::CSEntry rv = getEmpty();
	cache_[rv].set(s);
	return rv;
}

CacheString::CSEntry CSCache::add(const std::string& s)
{
	CacheString::CSEntry rv = getEmpty();
	cache_[rv].set(s);
	return rv;
}

const char* CSCache::get(CacheString::CSEntry entry)
{
	assert(entry < allocated_);
	return cache_[entry].get();
}

CacheString::CSEntry CSCache::ref(CacheString::CSEntry entry)
{
	assert(entry < allocated_);
	if (entry != Nil)
		cache_[entry].ref();

	return entry;
}

CacheString::CSEntry CSCache::unref(CacheString::CSEntry entry)
{
	assert(entry < allocated_);
	if (entry != Nil && cache_[entry].unref()) {
		empty_bucket_.push(&cache_[entry]);
	}

	return entry;
}

size_t CSCache::size(CacheString::CSEntry entry)
{
	assert(entry < allocated_);
	return cache_[entry].size();
}
