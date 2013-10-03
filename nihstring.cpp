#include <cassert>
#include "solvespace.h"
#include "nihstring.h"

NihCacheEntry NihCacheEntry::newNihCacheEntry(size_t index)
{
	NihCacheEntry rv = { NULL, 0, 0, index };
	return rv;
}

NihCacheEntry& NihCacheEntry::set(const char *s)
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

NihCacheEntry& NihCacheEntry::set(const std::string& s)
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

const char* NihCacheEntry::get()
{
	return str_;
}

void NihCacheEntry::free()
{
	assert(count_ == 0);

	MemFree(str_);
	size_ = 0;
	str_ = NULL;
}

void NihCacheEntry::retain()
{
	assert(count_ > 0);
	count_++;
}

bool NihCacheEntry::release()
{
	assert(count_ > 0);
	count_--;
	if (count_ == 0) {
		free();
		return true;
	}

	return false;
}

size_t NihCacheEntry::size()
{
	return size_;
}

NihCache::NihEntry NihCache::getEmpty()
{
	if (empty_bucket_.empty()) {
		size_t new_allocated = allocated_ * 2;
		cache_ = static_cast<NihCacheEntry*>(MemRealloc(cache_, new_allocated * sizeof(*cache_)));
		populateBucket(allocated_, new_allocated);
		allocated_ = new_allocated;
	}

	NihCache::NihEntry rv = empty_bucket_.front()->index_;
	empty_bucket_.pop();

	return rv;
}

NihCache::NihEntry NihCache::add(const char* s)
{
	NihCache::NihEntry rv = getEmpty();
	cache_[rv].set(s);
	//printf("%s: %s, index: %lu\n", __func__, s, rv);
	return rv;	
}

NihCache::NihEntry NihCache::add(const std::string& s)
{
	NihCache::NihEntry rv = getEmpty();
	cache_[rv].set(s);
	//printf("%s: %s, index: %lu\n", __func__, s.c_str(), rv);
	return rv;
}

const char* NihCache::get(NihEntry entry)
{
	assert(entry < allocated_);

	//printf("%s: %s, index: %lu\n", __func__, cache_[entry].get(), entry);

	return cache_[entry].get();
}

NihCache::NihEntry NihCache::retain(NihEntry entry)
{
	assert(entry < allocated_);
	if (entry != Nil)
		cache_[entry].retain();

	//printf("%s: index: %lu\n", __func__, entry);

	return entry;
}

NihCache::NihEntry NihCache::release(NihEntry entry)
{
	assert(entry < allocated_);
	if (entry != Nil && cache_[entry].release()) {
		empty_bucket_.push(&cache_[entry]);
	}

	//printf("%s: index: %lu\n", __func__, entry);

	return entry;
}

size_t NihCache::size(NihEntry entry)
{
	assert(entry < allocated_);
	return cache_[entry].size();
}
