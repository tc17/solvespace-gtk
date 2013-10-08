#ifndef NIHSTRING_H
#define NIHSTRING_H 1

#include <string>
#include <queue>

/* POD */
struct NihCacheEntry {
	char *str_;
	int count_;
	size_t size_;
	size_t index_;

	static NihCacheEntry newNihCacheEntry(size_t index);
	NihCacheEntry& set(const char *s);
	NihCacheEntry& set(const std::string& s);
	const char* get();
	void free();
	void retain();
	bool release();
	size_t size();
};

class NihCache {
	static const size_t preallocated = 32;
	size_t allocated_;
	NihCacheEntry *cache_;
	std::queue<NihCacheEntry*> empty_bucket_;
public:
	typedef size_t NihEntry; // should be POD
	static const NihEntry Nil = 0;

	static NihCache& getInstance()
	{
		static NihCache instance;
		return instance;
	}

	NihEntry add(const char* s);
	NihEntry add(const std::string& s);
	const char* get(NihEntry entry);
	NihEntry retain(NihEntry entry);
	NihEntry release(NihEntry entry);
	size_t size(NihEntry entry);

private:
	NihCache() : allocated_(preallocated), 
			cache_(static_cast<NihCacheEntry*>(MemAlloc(sizeof(*cache_) * preallocated))),
			empty_bucket_()
	{
		populateBucket(0, allocated_);
		add(NULL); // cache_[0] special Nil entry
	}

	~NihCache()
	{
		for (size_t i = 0; i < allocated_; i++)
			cache_[i].free();
		MemFree(cache_);
	}

	NihEntry getEmpty();

	NihCache(const NihCache&);
	NihCache& operator=(const NihCache&);

	void populateBucket(size_t start, size_t end)
	{
		for (size_t i = start; i < end; i++) {
			cache_[i] = NihCacheEntry::newNihCacheEntry(i);
			empty_bucket_.push(&cache_[i]);
		}
	}
};

/* POD */
struct NihString {
	NihCache::NihEntry entry_;

	static NihString newNihString(const char *s)
	{
		NihString rv;
		rv.entry_ = NihCache::getInstance().add(s);
		return rv;
	}

	static NihString newNihString(const std::string& s)
	{
		NihString rv;
		rv.entry_ = NihCache::getInstance().add(s);
		return rv;
	}

	NihString& retain()
	{
		NihCache::getInstance().retain(entry_);
		return *this;
	}

	NihString& release()
	{
		NihCache::getInstance().release(entry_);
		return *this;
	}

	const char* c_str()
	{
		return NihCache::getInstance().get(entry_);
	}

	std::string std_str()
	{
		const char *str = NihCache::getInstance().get(entry_);
		return str ? std::string(str) : std::string();
	}

	bool empty()
	{
		return NihCache::getInstance().size(entry_) == 0;
	}
};

#endif /* NIHSTRING */
