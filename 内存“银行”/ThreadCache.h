#include"Common.h"
#include"CentralCache.h"
class ThreadCache
{
public:
	//�����ڴ���ͷ��ڴ�
	void* Allocte(size_t size);
	void Deallocte(void* ptr, size_t size);

	//�����Ļ����ȡ����
	void* FetchFromCentralCache(size_t index);

	//�������������һ�����Ⱦ�Ҫ�ͷŸ����Ļ���
	void ListTooLong(FreeList& freeList, size_t num, size_t size);
private:
	FreeList _freeLists[NFREE_LIST];
};

_declspec(thread) static ThreadCache* pThreadCache = nullptr;