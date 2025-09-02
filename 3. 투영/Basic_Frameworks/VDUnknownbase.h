#pragma once
#include <Windows.h>
#include "VDGUID.h"

// VDIUnknown 전방 선언
__interface VDIUnknownBase
{
	virtual int QueryInterface(VDIID rVDIID, _Out_ void** ppvObject) = 0; // COM 인터페이스를 가져오는 메서드
	virtual ULONG AddRef() = 0; // COM 객체의 참조 카운트를 증가
	virtual ULONG Release() = 0; // COM 객체의 참조 카운트를 감소
};

// IUnknownBase를 상속받는 클래스에서 COM 객체를 가져오는 템플릿 함수
template <class T>
T* GetClassObject(VDIUnknownBase* vpInterface)
{
	T* pObject = dynamic_cast<T*>(vpInterface); // pUnknown을 T 타입으로 캐스팅

	if (pObject == nullptr) { return nullptr; } // 캐스팅 실패 시 nullptr 반환

	return pObject;
}