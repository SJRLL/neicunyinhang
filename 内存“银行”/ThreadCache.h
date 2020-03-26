#include"Common.h"
#include"CentralCache.h"
class ThreadCache
{
public:
	//申请内存和释放内存
	void* Allocte(size_t size);
	void Deallocte(void* ptr, size_t size);

	//从中心缓存获取对象
	void* FetchFromCentralCache(size_t index);

	//如果自由链表超过一定长度就要释放给中心缓存
	void ListTooLong(FreeList& freeList, size_t num, size_t size);
private:
	FreeList _freeLists[NFREE_LIST];
};

_declspec(thread) static ThreadCache* pThreadCache = nullptr;