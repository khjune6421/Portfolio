#pragma once
#include "VDUnknownbase.h"
#include "VDX9types.h"
#include "VDMath.h"

__interface IVooDoo; // VooDoo 인터페이스 전방 선언
__interface IVooDooDevice; // VooDooDevice 인터페이스 전방 선언
__interface IVooDooVertexBuffer; // VooDooVertexBuffer 인터페이스 전방 선언

// VooDoo 인터페이스 정의
__interface IVooDooDevice : public VDIUnknownBase
{
	virtual int BeginScene() = 0; // 씬을 시작하는 메서드
	virtual int EndScene() = 0; // 씬을 끝내는 메서드
	virtual int Clear(COLORREF col) = 0; // 화면을 지우는 메서드
	virtual int Present() = 0; // 현재 씬을 화면에 표시하는 메서드

	virtual int CreateVertexBuffer(UINT Size, DWORD Usage, DWORD FVF, VDPOOL Pool, _Out_ IVooDooVertexBuffer** ppVertexBuffer, _In_opt_ HANDLE* pSharedHandle) = 0; // 버텍스 버퍼를 생성하는 메서드
	virtual int SetStreamSource(UINT StreamNumber, IVooDooVertexBuffer* pStreamData, UINT OffsetInBytes, UINT Stride) = 0; // 스트림 소스를 설정하는 메서드 // 뭔지 모르겠음
	virtual int SetFVF(DWORD FVF) = 0; // FVF를 설정하는 메서드
	virtual int DrawPrimitive(VDPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) = 0; // 지정된 형태를 그리는 메서드

	virtual int SetRenderState(VDRENDERSTATETYPE State, DWORD Value) = 0; // 렌더 상태를 설정하는 메서드
	virtual int GetRenderState(VDRENDERSTATETYPE State, _Out_ DWORD* pValue) = 0; // 렌더 상태를 가져오는 메서드

	virtual int SetTransform(VDTRANSFORMSTATETYPE State, const VDMATRIX* pMatrix) = 0; // 변환 행렬을 설정하는 메서드
	virtual int GetTransform(VDTRANSFORMSTATETYPE State, _Out_ VDMATRIX* pMatrix) = 0; // 변환 행렬을 가져오는 메서드

	virtual HDC GetRT() = 0; // 현재 렌더 타겟의 DC를 가져오는 메서드
	virtual COLORREF GetClearColor() = 0; // 현재 클리어 색상을 가져오는 메서드

	virtual void DrawGrid() = 0; // 그리드 그리기 메서드
	virtual void DrawAxis() = 0; // 축 그리기 메서드
};

// VooDooDevice 인터페이스 정의
typedef __interface IVooDooVertexBuffer : public VDIUnknownBase
{
	virtual int Lock(UINT OffsetToLock, UINT SizeToLock, _Out_ void** ppbData, DWORD Flags) = 0; // 버퍼를 잠그는 메서드
	virtual int Unlock() = 0; // 버퍼의 잠금을 해제하는 메서드
	virtual int GetDesc(_Out_ VDVERTEXBUFFER_DESC* pDesc) = 0; // 버퍼의 속성을 가져오는 메서드(Size, Usage, Pool)

	virtual void* GetBuffer() = 0; // 버퍼를 가져오는 메서드
	virtual UINT GetVertexAmount() = 0; // 버퍼의 정점 개수를 가져오는 메서드
	virtual int GetPrivateData(_Out_ void** ppBuffer, _Out_ UINT* pSize) = 0; // 마찬가지로 프라이빗 데이터를 가져오는 메서드 // 어디에 쓰는지, 왜 상속하는지 모르겠음
};

typedef __interface IVooDoo : public VDIUnknownBase
{
		virtual int CreateDevice(HWND hWnd, VDPRESENT_PARAMETERS* pp, DWORD vp, _Out_ IVooDooDevice** ppDevice) = 0; // 디바이스를 생성하는 메서드
};

// VooDoo 객체를 생성하는 함수 // 왜 VooDoo.h에 정의하지 않고 따로 분리해야 하는지는 모르겠음
IVooDoo* VooDooCreate(DWORD ver);