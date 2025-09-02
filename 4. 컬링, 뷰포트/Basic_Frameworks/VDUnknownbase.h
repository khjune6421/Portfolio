#pragma once
#include <Windows.h>
#include "VDGUID.h"

// VDIUnknown ���� ����
__interface VDIUnknownBase
{
	virtual int QueryInterface(VDIID rVDIID, _Out_ void** ppvObject) = 0; // COM �������̽��� �������� �޼���
	virtual ULONG AddRef() = 0; // COM ��ü�� ���� ī��Ʈ�� ����
	virtual ULONG Release() = 0; // COM ��ü�� ���� ī��Ʈ�� ����
};

// IUnknownBase�� ��ӹ޴� Ŭ�������� COM ��ü�� �������� ���ø� �Լ�
template <class T>
T* GetClassObject(VDIUnknownBase* vpInterface)
{
	T* pObject = dynamic_cast<T*>(vpInterface); // pUnknown�� T Ÿ������ ĳ����

	if (pObject == nullptr) { return nullptr; } // ĳ���� ���� �� nullptr ��ȯ

	return pObject;
}