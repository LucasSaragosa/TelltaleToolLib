// This file was written by Lucas Saragosa. The code derives from Telltale Games' Engine.
// I do not intend to take credit for it, however; Im the author of this interpretation of 
// the engine and require that if you use this code or library, you give credit to me and
// the amazing Telltale Games.

#include "Container.h"
#include "../Meta.hpp"
#include <deque>

#ifndef _DEQUE
#define _DEQUE

template<typename T> class Deque : public ContainerInterface, public std::deque<T>, public ContainerDataAttach<T> {
public:
	virtual int GetSize() {
		return this->size();
	}

	//no need for serialize main

	static MetaOpResult MetaOperation_SerializeAsync(void* pObj,
		MetaClassDescription* pDesc, MetaMemberDescription* mCtx, void* pUserData) {
		Deque<T>* array = static_cast<Deque<T>*>(pObj);
		MetaClassDescription* vtype = ::GetMetaClassDescription(typeid(T).name());
		if (!array || !vtype)return eMetaOp_Fail;
		MetaOperation op = vtype->GetOperationSpecialization(74);
		if (!op)op = Meta::MetaOperation_SerializeAsync;
		unsigned int size = array->GetSize();
		static_cast<MetaStream*>(pUserData)->serialize_uint32(&size);
		for (int i = 0; i < size; i++) {
			if (static_cast<MetaStream*>(pUserData)->mMode == MetaStreamMode::eMetaStream_Write) {
				const T v = array->operator[](i);
				if (op((void*)&v, vtype, NULL, pUserData) != eMetaOp_Succeed)return eMetaOp_Fail;
			}
			else {
				T value = T();
				if (op(&value, vtype, NULL, pUserData) != eMetaOp_Succeed)return eMetaOp_Fail;
				array->AddElement(0, NULL, &value);//OK, since its a copy
			}
		}
		return eMetaOp_Succeed;
	}

	INLINE virtual bool KeyedContainer() { return false; }
	INLINE virtual void* GetKey(int) { return NULL; }

	virtual MetaClassDescription* GetContainerKeyClassDescription() {
		return NULL;
	}

	virtual MetaClassDescription* GetContainerDataClassDescription() {
		return  this->mpContainerDataType;
	}

	const T operator[](int at_index) {
		int i = 0;
		for (auto it = this->begin(); it != this->end(); it++, i++) {
			if (i == at_index) {
				return *it;
				break;
			}
		}
		return *(T*)NULL;
	}

	INLINE virtual bool IsResizable() { return true; }

	virtual bool AllocateElements(int total) {
		return false;
	}

	virtual void ClearElements() {
		this->clear();
	}

	virtual void RemoveElement(int at_index) {
		int i = 0;
		for (auto it = this->begin(); it != this->end(); it++, i++) {
			if (i == at_index) {
				this->erase(it);
				break;
			}
		}
	}

	virtual void AddElement(int at_index, const void* pKeyData, void* pValueToAdd) {
		this->push_back(*(T*)pValueToAdd);
	}

	//DO NOT USE THIS IN DEQUES (NOT IMPLEMENTED)!
	virtual void SetElement(int at_index, const void* pKeyData, void* pValue) {}

	//DO NOT USE THIS IN DEQUES (NOT IMPLEMENTED)!
	virtual String* GetElementName(String* result, int index) {
		return NULL;
	}

	//DO NOT USE THIS IN DEQUES (NOT IMPLEMENTED)!
	virtual void* GetElement(int at_index) {
		return NULL;
	}
};

#endif