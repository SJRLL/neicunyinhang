#pragma once

#include "ThreadCache.h"
#include "PageCache.h"

static void* ConcurrentMalloc(size_t size)
{
	if (size <= MAX_SIZE) // [1byte, 64kb] --->找Thread Cache
	{
		if (pThreadCache == nullptr)
		{
			pThreadCache = new ThreadCache;
			cout << std::this_thread::get_id() << "->" << pThreadCache << endl;
		}

		return pThreadCache->Allocte(size);
	}
	else if (size <= ((MAX_PAGES - 1) << PAGE_SHITF)) // (64kb, 128*4kb] --->找Central Cache
	{
		size_t align_size = SizeClass::_RoundUp(size, 1 << PAGE_SHITF);
		size_t pagenum = (align_size >> PAGE_SHITF);
		Span* span = PageCache::GetPageCacheInstance().NewSpan(pagenum);
		span->_objsize = align_size;
		void* ptr = (void*)(span->_pageid << PAGE_SHITF);
		return ptr;
	}
	else // [128*4kb,-]  --->大于128*4kb找系统申请
	{
		size_t align_size = SizeClass::_RoundUp(size, 1 << PAGE_SHITF);
		size_t pagenum = (align_size >> PAGE_SHITF);
		return SystemAlloc(pagenum);
	}
}

static void ConcurrentFree(void* ptr)
{
	size_t pageid = (PAGE_ID)ptr >> PAGE_SHITF;
	Span* span = PageCache::GetPageCacheInstance().GetIdToSpan(pageid);
	if (span == nullptr)// [128*4kb,-]
	{
		SystemFree(ptr);
		return;
	}

	size_t size = span->_objsize;
	if (size <= MAX_SIZE) // [1byte, 64kb]
	{
		pThreadCache->Deallocte(ptr, size);
	}
	else if (size <= ((MAX_PAGES - 1) << PAGE_SHITF)) // (64kb, 128*4kb]
	{
		PageCache::GetPageCacheInstance().ReleaseSpanToPageCache(span);
	}
}
