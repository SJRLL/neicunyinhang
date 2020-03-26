#include "PageCache.h"
#include "CentralCache.h"

//��spanlist ����Page Cache��ȡһ��span
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

	// page cache ��ȡһ��span
	size_t numpage = SizeClass::NumMovePage(size);
	Span* span = PageCache::GetPageCacheInstance().NewSpan(numpage);
	// ��span�����гɶ�Ӧ��С�ҵ�span��freelist��
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

//�����Ļ����ȡһ�������Ķ����ThreadCache
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

//��һ�������Ķ����ͷŵ�span���
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

		// ��ʾ��ǰspan�г�ȥ�Ķ���ȫ�����أ����Խ�span����page cache,���кϲ��������ڴ���Ƭ��
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