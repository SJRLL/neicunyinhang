#include "PageCache.h"
#include "CentralCache.h"

//从spanlist 或者Page Cache获取一个span
Span* CentralCache::GetOneSpan(size_t size)
{
	size_t index = SizeClass::ListIndex(size);
	SpanList& spanlist = _spanlists[index];
	Span* it = spanlist.Begin();
	while (it != spanlist.End())
	{
		if (!it->_freelist.Empty())
		{
			return it;
		}
		else
		{
			it = it->_next;
		}
	}

	// page cache 获取一个span
	size_t numpage = SizeClass::NumMovePage(size);
	Span* span = PageCache::GetPageCacheInstance().NewSpan(numpage);
	// 把span对象切成对应大小挂到span的freelist中
	char* start = (char*)(span->_pageid << 12);
	char* end = start + (span->_pagesize << 12);
	while (start < end)
	{
		char* obj = start;
		start += size;

		span->_freelist.Push(obj);
	}
	span->_objsize = size;
	spanlist.PushFront(span);

	return span;
}

//从中心缓存获取一定数量的对象给ThreadCache
size_t CentralCache::FetchRangeObj(void*& start, void*& end, size_t num, size_t size)
{
	size_t index = SizeClass::ListIndex(size);
	SpanList& spanlist = _spanlists[index];
	spanlist.Lock();

	Span* span = GetOneSpan(size);
	FreeList& freelist = span->_freelist;
	size_t actualNum = freelist.PopRange(start, end, num);
	span->_usecount += actualNum;

	spanlist.Unlock();

	return actualNum;
}

//将一定数量的对象释放到span跨度
void CentralCache::ReleseListToSpans(void* start, size_t size)
{
	size_t index = SizeClass::ListIndex(size);
	SpanList& spanlist = _spanlists[index];
	spanlist.Lock();

	while (start)
	{
		void* next = NextObj(start);
		PAGE_ID id = (PAGE_ID)start >> PAGE_SHITF;
		Span* span = PageCache::GetPageCacheInstance().GetIdToSpan(id);
		span->_freelist.Push(start);
		span->_usecount--;

		// 表示当前span切出去的对象全部返回，可以将span还给page cache,进行合并，减少内存碎片。
		if (span->_usecount == 0)
		{
			size_t index = SizeClass::ListIndex(span->_objsize);
			_spanlists[index].Erase(span);
			span->_freelist.Clear();

			PageCache::GetPageCacheInstance().ReleaseSpanToPageCache(span);
		}

		start = next;
	}

	spanlist.Unlock();
}