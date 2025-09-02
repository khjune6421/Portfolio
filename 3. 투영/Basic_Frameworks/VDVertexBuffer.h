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
	ULONG m_refCount = 0; // 참조 카운트

	void* m_pVertexBuffer; // 정점 버퍼
	DWORD m_sizeInBytes; // 버퍼 크기 (바이트 단위)
	DWORD m_FVF;
	UINT m_Stride; // 스트라이드 크기 (바이트 단위)
	UINT m_vertexAmount; // 정점 개수
	BOOL m_isLocked; // 버퍼 잠금 상태

	VDVERTEXBUFFER_DESC m_desc; // 버퍼 속성 구조체

	// 버퍼 운영 함수/메서드
	int _Create(VDVERTEXBUFFER_DESC desc); // 버퍼 생성 함수
	int _Create(UINT length, DWORD FVF, VDPOOL Pool);

	void* _GetBuffer() { return m_pVertexBuffer; } // 버퍼를 가져오는 함수
	DWORD _GetSizeInBytes() { return m_sizeInBytes; } // 버퍼 크기를 가져오는 함수
	DWORD _GetFVF() { return m_FVF; } // FVF를 가져오는 함수
	DWORD _GetStride() { return m_Stride; } // 스트라이드 크기를 가져오는 함수
	UINT _GetVertexCount() { return m_vertexAmount; } // 정점 개수를 가져오는 함수

	// 정점 접근 메서드
	VDVECTOR _GetVertexPosition(UINT index) const; // 정점 위치를 가져오는 메서드
	DWORD _GetDiffuse(UINT index) const; // 정점 색상을 가져오는 메서드

public:
	VDVertexBuffer(); // 생성자
	virtual ~VDVertexBuffer(); // 소멸자

	virtual int Lock(UINT OffsetToLock, UINT SizeToLock, _Out_ void** ppbData, DWORD Flags) override; // 버퍼를 잠그는 메서드
	virtual int Unlock() override; // 버퍼의 잠금을 해제하는 메서드
	virtual int GetDesc(_Out_ VDVERTEXBUFFER_DESC* pDesc) override; // 버퍼의 속성을 가져오는 메서드(Size, Usage, Pool)

	virtual void* GetBuffer() override { return m_pVertexBuffer; } // 버퍼를 가져오는 메서드
	virtual UINT GetVertexAmount() override { return m_vertexAmount; } // 버퍼의 정점 개수를 가져오는 메서드
	virtual int GetPrivateData(_Out_ void** ppBuffer, _Out_ UINT* pSize) override;

	virtual ULONG AddRef() { return ++m_refCount; } // 참조 카운트 증가
	virtual ULONG Release() { if (--m_refCount <= 0) { delete this; return 0; } return m_refCount; }
	virtual int QueryInterface(VDIID rVDIID, _Out_ void** ppvObject);
};