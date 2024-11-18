#pragma once
#include "../TelltaleToolLibrary.h"

/**
 * Linear heap is a classic game engine design. A paged memory allocated that supports holding destructors and calling them when a page is released.
 * This is not just a memory allocator, but an OOP supported GC.
 */
class LinearHeap {

	struct Page {
		u32 mSize;
		u32 mIndex;
		Page* mpNext;
	};

	struct ObjWrapperBase {

		ObjWrapperBase* mpNext;
		
		inline virtual ~ObjWrapperBase() {}

	};

	template<typename T>
	struct ObjWrapper : ObjWrapperBase {

		T mObj;

		inline virtual ~ObjWrapper() {}

	};

	template<typename T>
	struct ObjArrayWrapper : ObjWrapperBase {

		u32 mSize;
		T* mArray;

		inline virtual ~ObjArrayWrapper() {}

	};

	struct Context {

		ObjWrapperBase* mpFirstObject, * mpLastObject;
		Page* mpPage;
		u32 mPagePos;
		u32 mObjCount;
		Context* mpNext;

	};

	u32 mPageCount;
	u32 mTotalMemUsed;
	u32 mPageSize;
	u32 mCurrentPos;
	Context* mpContextStack;
	Context mBaseContext;
	Page* mpBasePage, * mpCurrentPage;

	inline Page* _AllocatePage(u32 size = 0){
		size = max(size, mPageSize);
		Page* pg = (Page*)_aligned_malloc(size, 32);
		if (!pg)
			return 0;
		memset(pg, 0, size);
		pg->mIndex = mPageCount++;
		pg->mSize = size;
		pg->mpNext = 0;
		mTotalMemUsed += size;
		return pg;
	}

	inline void _ReleasePageList(Page* pPage) {
		while(pPage){
			Page* next = pPage->mpNext;
			_aligned_free(pPage);
			pPage = next;
		}
	}

	inline void _CallDestructors(Context& context){
		ObjWrapperBase* pObject = context.mpFirstObject;
		while(pObject){
			ObjWrapperBase* next = pObject->mpNext;
			next->~ObjWrapperBase();
			pObject = next;
		}
		context.mpLastObject = context.mpFirstObject = NULL;
		context.mObjCount = 0;
	}

	inline void _AddObject(ObjWrapperBase* pObj){
		if(mpContextStack->mpLastObject)
			mpContextStack->mpLastObject->mpNext = pObj;
		if (!mpContextStack->mpFirstObject)
			mpContextStack->mpFirstObject = pObj;
		mpContextStack->mpLastObject = pObj;
		++mpContextStack->mObjCount;
	}

	inline void _PopContextInt(){
		_CallDestructors(*mpContextStack);
		mpContextStack->mpFirstObject = mpContextStack->mpLastObject = 0;
		mpContextStack->mObjCount = 0;
		mpCurrentPage = mpContextStack->mpPage;
		mCurrentPos = mpContextStack->mPagePos;
		mpContextStack = mpContextStack->mpNext;
	}
	
public:

	inline LinearHeap(u32 pageSize = 0x100000) : mpContextStack(&mBaseContext), mpCurrentPage(0), mpBasePage(0), 
		mTotalMemUsed(0), mPageSize(pageSize), mCurrentPos(0), mPageCount(0) {}
	
	inline ~LinearHeap() {
		ReleaseAll();
	}

	/**
	 * Allocate size number of bytes with a optional align. Align can be from 1 to 32 and must be a power of two (1,2,4,8,16,32,...).
	 */
	inline char* Alloc(u32 size, u32 align = 1){
		Page* currentPage = mpCurrentPage;
		char* pRet = 0;
		if(currentPage){
			u32 alignedOffset = (mCurrentPos + (align - 1)) & ~(align - 1);
			if (alignedOffset + size <= currentPage->mSize){
				//OK found a page!
				pRet = (char*)currentPage + sizeof(Page) + alignedOffset;
				mCurrentPos = size + alignedOffset;
			}else{
				Page* pNext = _AllocatePage(size);
				mCurrentPos = size;
				pNext->mpNext = currentPage;
				mpCurrentPage = pNext;
				pRet = (char*)pNext + sizeof(Page);
			}
		}else{
			mpCurrentPage = currentPage = _AllocatePage(size);
			pRet = (char*)currentPage + sizeof(Page);
			mCurrentPos = size;
		}
		return pRet;
	}

	/**
	 * Allocates and returns T. Calls default ctor. The destructor will be called when you pop the current context.
	 */
	template<typename T>
	inline T* New() {
		ObjWrapper<T>* pAlloc = (ObjWrapper<T>*)Alloc(sizeof(ObjWrapper<T>), alignof(ObjWrapper<T>));
		pAlloc->mpNext = 0;
		new ((char*)(&pAlloc->mObj)) T();
		_AddObject(pAlloc);
		return &pAlloc->mObj;
	}

	/**
	 * See New. Allocates an array of numElem consecutively in memory and returns it. Destructors will be called when the current context is popped.
	 */
	template<typename T>
	inline T* NewArray(u32 numElem) {
		ObjArrayWrapper<T>* pArrayWrapper = (ObjArrayWrapper<T>*)Alloc(sizeof(ObjArrayWrapper<T>), alignof(ObjArrayWrapper<T>));
		pArrayWrapper->mArray = NewArrayNoDestruct<T>(numElem);
		pArrayWrapper->mpNext = 0;
		_AddObject(pArrayWrapper);
		return pArrayWrapper->mArray;
	}

	/**
	 * Creates a new instance of T in this linear heap. Its constructor will not be called at all by this class so you will need to if you wish.
	 */
	template<typename T>
	inline T* NewNoDestruct() {
		char* pAlloc = Alloc(sizeof(T), alignof(T));
		new (pAlloc) T();
		return (T*)pAlloc;
	}

	/**
	 * See NewNoDestruct. Array version. Consecutive in memory.
	 */
	template<typename T>
	inline T* NewArrayNoDestruct(u32 numElem) {
		char* pAlloc = Alloc(sizeof(T) * numElem, alignof(T));
		for (u32 i = 0; i < numElem; i++)
			new (pAlloc + (i * sizeof(T))) T();
		return (T*)pAlloc;
	}

	/**
	 * Pushes a destructor context. This means all objects returned from New and NewArray (without NoDestruct) will have their destructors called at the next PopContext,
	 * which will pop this newly pushed context.
	 */
	inline void PushContext() {
		Context* pContext = NewNoDestruct<Context>();
		pContext->mpNext = mpContextStack;
		mpContextStack = pContext;
	}

	/**
	 * Pops a context. This will call destructors of any objects created using New or NewArray between the last call of PushContext and this call.
	 */
	inline void PopContext() {
		if(mpContextStack && mpContextStack->mpNext){
			_PopContextInt();
			if (!mpCurrentPage)
				mpCurrentPage = mpBasePage;
		}
	}

	/**
	 * Gets the index of the base page in this linear heap. Return -1 if no pages.
	 */
	inline int GetBasePageIndex() {
		return mpBasePage ? mpBasePage->mIndex : -1;
	}

	/**
	 * Returns the number of bytes free in the current page - anymore would require a new allocation.
	 */
	inline int GetCurrentPageBytesFree(){
		return mpCurrentPage ? mpCurrentPage->mSize - mCurrentPos : mPageSize;
	}

	/**
	 * Gets the page index for a given pointer, checking the pointer is in the range of one the pages. Else returns -1.
	 */
	inline int GetPageIndexForAlloc(void* pAlloc){
		for(Page* i = mpBasePage; i; i = i->mpNext){
			if (pAlloc >= ((u8*)i + sizeof(Page)) && pAlloc <= ((u8*)i + sizeof(Page) + i->mSize))
				return i->mIndex;
		}
		return -1;
	}

	/**
	 * Frees only the first page in this linear heap.
	 */
	inline void FreeFirstPage() {
		if(mpBasePage){
			Page* b = mpBasePage->mpNext;
			_aligned_free(mpBasePage);
			mpBasePage = b;
			mPageCount--;
		}
		if(!mPageCount){
			mTotalMemUsed = 0;
			mpCurrentPage = 0;
		}
	}

	/**
	 * Pops all contexts in this linear heap, calling all destructors.
	 */
	inline void FreeAll() {
		while (mpContextStack && mpContextStack->mpNext)
			PopContext();
		if (mpContextStack)
			_PopContextInt();
		mpContextStack = &mBaseContext;
	}

	/**
	 * Releases all memory associated with this linear heap, along with popping all contexts and calling destructors.
	 */
	inline void ReleaseAll(){
		FreeAll();
		_ReleasePageList(mpBasePage);
	}

	/**
	 * Returns a string allocated with the contents of the given constant string. Length can be 0 in which the length will be calculated. The modifiable string is returned.
	 */
	inline char* StringIntern(const char* s, u32 length = 0){
		if (!length)
			length = strlen(s);
		char* pAlloc = Alloc(length + 1, 1);
		memcpy(pAlloc, s, length);
		pAlloc[length] = 0;
		return pAlloc;
	}

};