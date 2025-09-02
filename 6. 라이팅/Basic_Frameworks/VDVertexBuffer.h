#pragma once
#include <Windows.h>
#include "VDX9types.h"
#include "VDMath.h"
#include "VDGUID.h"
#include "VDX9.h"

class VDVertexBuffer : public IVooDooVertexBuffer
{
	friend class VooDooDevice;
	friend class VooDooGraphics;

protected:
	ULONG m_refCount = 0; // ���� ī��Ʈ

	void* m_pVertexBuffer; // ���� ����
	DWORD m_sizeInBytes; // ���� ũ�� (����Ʈ ����)
	DWORD m_FVF;
	UINT m_Stride; // ��Ʈ���̵� ũ�� (����Ʈ ����)
	UINT m_vertexAmount; // ���� ����
	BOOL m_isLocked; // ���� ��� ����

	VDVERTEXBUFFER_DESC m_desc; // ���� �Ӽ� ����ü

	// ���� � �Լ�/�޼���
	int _Create(VDVERTEXBUFFER_DESC desc); // ���� ���� �Լ�
	int _Create(UINT length, DWORD FVF, VDPOOL Pool);

	void* _GetBuffer() { return m_pVertexBuffer; } // ���۸� �������� �Լ�
	DWORD _GetSizeInBytes() { return m_sizeInBytes; } // ���� ũ�⸦ �������� �Լ�
	DWORD _GetFVF() { return m_FVF; } // FVF�� �������� �Լ�
	DWORD _GetStride() { return m_Stride; } // ��Ʈ���̵� ũ�⸦ �������� �Լ�
	UINT _GetVertexCount() { return m_vertexAmount; } // ���� ������ �������� �Լ�

	// ���� ���� �޼���
	VDVECTOR _GetVertexPosition(UINT index) const; // ���� ��ġ�� �������� �޼���
	DWORD _GetDiffuse(UINT index) const; // ���� ������ �������� �޼���

public:
	VDVertexBuffer(); // ������
	virtual ~VDVertexBuffer(); // �Ҹ���

	virtual int Lock(UINT OffsetToLock, UINT SizeToLock, _Out_ void** ppbData, DWORD Flags) override; // ���۸� ��״� �޼���
	virtual int Unlock() override; // ������ ����� �����ϴ� �޼���
	virtual int GetDesc(_Out_ VDVERTEXBUFFER_DESC* pDesc) override; // ������ �Ӽ��� �������� �޼���(Size, Usage, Pool)

	virtual void* GetBuffer() override { return m_pVertexBuffer; } // ���۸� �������� �޼���
	virtual UINT GetVertexAmount() override { return m_vertexAmount; } // ������ ���� ������ �������� �޼���
	virtual int GetPrivateData(_Out_ void** ppBuffer, _Out_ UINT* pSize) override;

	virtual ULONG AddRef() { return ++m_refCount; } // ���� ī��Ʈ ����
	virtual ULONG Release() { if (--m_refCount <= 0) { delete this; return 0; } return m_refCount; }
	virtual int QueryInterface(VDIID rVDIID, _Out_ void** ppvObject);
};