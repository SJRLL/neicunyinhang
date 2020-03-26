#pragma once
#include"Common.h"
#include"PageCache.h"

class CentralCache
{
public:
	//从中心缓存获取一定数量的对象给ThreadCache
	size_t FetchRangeObj(void*& start, void*& end, size_t num, size_t size);

	//将一定数量的对象释放到span跨度
	void ReleseListToSpans(void* start, size_t size);

	//从spanlist 或者Page Cache获取一个span
	Span* GetOneSpan(size_t size);

	static CentralCache& GetCentralCacheInstance()
	{
		static CentralCache inst;
		return inst;
	}
private:

	CentralCache()
	{}

	CentralCache(const CentralCache&) = delete;

	SpanList _spanlists[NFREE_LIST];
};

//static CentralCache centralCacheInst;//静态区，线程共享