#pragma once
#include"Common.h"
#include"PageCache.h"

class CentralCache
{
public:
	//�����Ļ����ȡһ�������Ķ����ThreadCache
	size_t FetchRangeObj(void*& start, void*& end, size_t num, size_t size);

	//��һ�������Ķ����ͷŵ�span���
	void ReleseListToSpans(void* start, size_t size);

	//��spanlist ����Page Cache��ȡһ��span
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

//static CentralCache centralCacheInst;//��̬�����̹߳���