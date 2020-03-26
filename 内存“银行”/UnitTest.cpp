#define _CRT_SECURE_NO_WARNINGS 1
#include"Common.h"
#include"ConcurrentMalloc.h"
#include<vector>
void UnitTest()
{
	cout << SizeClass::RoundUp(6) << endl;
	cout << SizeClass::RoundUp(15) << endl;
	cout << endl;

	cout << SizeClass::ListIndex(1) << endl;
	cout << SizeClass::ListIndex(128) << endl;
}

void UnitTestSystemAlloc()
{
	void* ptr1 = SystemAlloc(1);
	void* ptr2 = SystemAlloc(1);

	PAGE_ID id1 = (PAGE_ID)ptr1 >> PAGE_SHITF;
	PAGE_ID id2 = (PAGE_ID)ptr2 >> PAGE_SHITF;


	void* ptrshift1 = (void*)(id1 << PAGE_SHITF);
	void* ptrshift2 = (void*)(id2 << PAGE_SHITF);

	//   PAGE_ID id = (PAGE_ID)ptr >> PAGE_SHITF;
	char* obj1 = (char*)ptr1;
	char* obj2 = (char*)ptr1 + 8;
	char* obj3 = (char*)ptr1 + 16;
	PAGE_ID idd1 = (PAGE_ID)obj1 >> PAGE_SHITF;
	PAGE_ID idd2 = (PAGE_ID)obj2 >> PAGE_SHITF;
	PAGE_ID idd3 = (PAGE_ID)obj3 >> PAGE_SHITF;
}

#include"ConcurrentMalloc.h"
void func()
{
	std::vector<void*> v;
	size_t size = 7;
	for (size_t i = 0; i <512; ++i)
	{
		v.push_back(ConcurrentMalloc(size));
	}
	v.push_back(ConcurrentMalloc(size));

	for (size_t i = 0; i < v.size(); ++i)
	{

	}

	for (auto ptr : v)
	{
		ConcurrentFree(ptr);
	}
	v.clear();
}

//int main()
//{
//		unittest();
//	
//		UnitTestSystemAlloc();
//		func();
//	
//	void* ptr1 = ConcurrentMalloc(1 << PAGE_SHITF);
//	void* ptr2 = ConcurrentMalloc(65 << PAGE_SHITF);
//	void* ptr3 = ConcurrentMalloc(129 << PAGE_SHITF);
//
//	ConcurrentFree(ptr1);
//	ConcurrentFree(ptr2);
//	ConcurrentFree(ptr3);
//	system("pause");
//	return 0;
//}