#pragma once
#include "VDGUID.h"
#include "VDUnknownbase.h"
#include "VDX9.h"
#include "VDX9types.h"
#include "VDMath.h"
#include "VDGraphics.h"

class VooDooDevice : public IVooDooDevice // VooDooDevice 인터페이스 구현
{
	friend class VooDoo; // VooDoo 클래스만이 VooDooDevice의 생성자를 호출할 수 있도록

protected:
	ULONG m_refCount = 1; // 참조 카운트

	HWND m_hWnd; // 윈도우 핸들
	DWORD m_vertexProcessing; // 디바이스 생성 플래그

	HDC m_hRT; // 디바이스 컨텍스트 핸들
	COLORREF m_clearColor; // 클리어 색상

	VooDooGraphics* m_pGraphics; // VooDooGraphics 포인터

	int _Create(HWND hWnd, VDPRESENT_PARAMETERS* pp, DWORD vp); // 디바이스 생성 함수

public:
	VooDooDevice();
	virtual ~VooDooDevice();

	virtual int BeginScene() override { m_pGraphics->_BeginScene(); return 0; }; // 씬 시작 // m_pGraphics의 BeginScene 호출
	virtual int EndScene() override { m_pGraphics->_EndScene(); return 0; };
	virtual int Clear(COLORREF col) override { m_clearColor = col; return m_pGraphics->_Clear(col); };
	virtual int Present() override { return m_pGraphics->_Present(); };

	virtual int CreateVertexBuffer(UINT Size, DWORD Usage, DWORD FVF, VDPOOL Pool, _Out_ IVooDooVertexBuffer** ppVertexBuffer, _In_opt_ HANDLE* pSharedHandle) override;
	virtual int SetStreamSource(UINT StreamNumber, IVooDooVertexBuffer* pStreamData, UINT OffsetInBytes, UINT Stride) override;
	virtual int SetFVF(DWORD FVF) override { return m_pGraphics->_SetFVF(FVF); }; // FVF 설정
	virtual int DrawPrimitive(VDPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) override;

	virtual int SetRenderState(VDRENDERSTATETYPE State, DWORD Value) override { return m_pGraphics->_SetRenderState(State, Value); }; // 렌더 상태 설정
	virtual int GetRenderState(VDRENDERSTATETYPE State, _Out_ DWORD* pValue) override { return m_pGraphics->_GetRenderState(State, pValue); }; // 렌더 상태 가져오기

	virtual int SetTransform(VDTRANSFORMSTATETYPE State, const VDMATRIX* pMatrix) override { return m_pGraphics->_SetTransform(State, pMatrix); }; // 변환 행렬 설정
	virtual int GetTransform(VDTRANSFORMSTATETYPE State, _Out_ VDMATRIX* pMatrix) override { return m_pGraphics->_GetTransform(State, pMatrix); }; // 변환 행렬 가져오기

	virtual int SetViewport(VDVIEWPORT* pViewport) override { return m_pGraphics->_SetViewport(pViewport); }; // 뷰포트 설정
	virtual int GetViewport(VDVIEWPORT* pViewport) override { return m_pGraphics->_GetViewport(pViewport); }; // 뷰포트 가져오기

	virtual HDC GetRT() override { return m_pGraphics->_GetDCRT(); }; // 현재 렌더 타겟의 DC 가져오기
	virtual COLORREF GetClearColor() override { return m_clearColor; }; // 현재 클리어 색상 가져오기

	virtual void DrawGrid() override { m_pGraphics->_DrawGrid(); }; // 그리드 그리기
	virtual void DrawAxis() { m_pGraphics->_DrawAxis(); }; // 축 그리기

	virtual ULONG AddRef() override { return ++m_refCount; }; // 참조 카운트 증가
	virtual ULONG Release() override { if (--m_refCount == 0) { delete this; return 0; } return m_refCount; } // 참조 카운트 감소 및 객체 삭제
	virtual int QueryInterface(VDIID rVDIID, void** ppvObject) override;
};

class VooDoo : public IVooDoo // VooDoo 인터페이스 구현
{
protected:
	ULONG m_refCount = 1; // 참조 카운트

public:
	VooDoo(); // 생성자 // 아직 기능 없음
	virtual ~VooDoo(); // 소멸자 // 마찬가지로 아직 기능 없음

	virtual int CreateDevice(HWND hWnd, VDPRESENT_PARAMETERS* pp, DWORD vp, _Out_ IVooDooDevice** ppDevice);

	virtual ULONG AddRef() override { return ++m_refCount; }; // 참조 카운트 증가
	virtual ULONG Release() override { if (--m_refCount == 0) { delete this; return 0; } return m_refCount; } // 참조 카운트 감소 및 객체 삭제
	virtual int QueryInterface(VDIID rVDIID, _Out_ void** ppvObject) override;
};