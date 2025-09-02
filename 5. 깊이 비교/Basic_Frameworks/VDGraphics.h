#pragma once
#include <vector>
#include "VDMath.h"
#include "VDX9types.h"
#include "VDX9.h"

struct VS_OUTPUT
{
	VDVECTOR Pos; // 위치
	VDCOLOR Color; // 색상
};

typedef std::vector<VS_OUTPUT> VS_OUTS; // 정점 처리 결과 저장 벡터

class VooDooCore
{
	friend class VooDooDevice;

public:
	VooDooCore(); // 생성자 // 아직 기능 없음
	virtual ~VooDooCore(); // 소멸자 // 마찬가지로 아직 기능 없음
};

class VooDooGraphics : public VooDooCore
{
	HPEN m_hPen = nullptr;
	HPEN m_hOldPen = nullptr;

	friend class VooDooDevice;

public:
	struct INPUT_SYSTEM
	{
		IVooDooVertexBuffer* pVertexBuffer[1]; // 입력 버퍼
		DWORD FVF;
		UINT Stride;

		VDPRIMITIVETYPE PrimitiveType;
		UINT PrimitiveCount; // 그릴 프리미티브 개수
		UINT StartVertex; // 시작 정점 인덱스
		UINT VertexAmount; // 정점 개수
	};
	struct STREAM_DBG
	{
		UINT currentVertexIndex; // 현재 정점 인덱스
		UINT currentPrimitiveIndex; // 현재 도형 형태 인덱스
		UINT currentPrimitiveDrawCount; // 현재 도형 그리는 개수
	};
	struct TRANSFORM
	{
		VDMATRIX transform[4]; // 변환 행렬 배열 // 0: World, 1: View, 2: Projection
	};
	struct PIPELINE_STATE
	{
		DWORD renderState[256]; // 렌더 상태 배열
	};
	struct OUTPUT_MERGE
	{
		HWND hWnd; // 출력 윈도우 핸들
		VDPRESENT_PARAMETERS presentParameters; // 출력 정보(해상도, 백 버퍼 크기 등)
		HBITMAP hBackBuffer; // 백 버퍼 핸들
		HDC hDC; // 디바이스 컨텍스트 핸들
		VDVIEWPORT m_viewport; // 뷰포트 구조체
		HRGN m_hRGN; // 현재 렌더 타겟의 영역 핸들
	};
	struct DEPTH_BUFFER
	{
		std::vector<float> depthData; // 깊이 버퍼 데이터
		UINT depthWidth; // 깊이 버퍼 너비
		UINT depthHeight; // 깊이 버퍼 높이
	};

protected:
	IVooDooDevice* m_pDevice; // VooDoo 디바이스 포인터
	INPUT_SYSTEM m_inputSystem; // 입력 시스템 구조체
	STREAM_DBG m_streamDbg; // 스트림 디버그 구조체
	TRANSFORM m_transform; // 변환 행렬 구조체
	PIPELINE_STATE m_pipelineState; // 파이프라인 상태 구조체
	OUTPUT_MERGE m_outputMerge; // 출력 병합 구조체
	DEPTH_BUFFER m_depthBuffer; // 깊이 버퍼 구조체

public:
	VooDooGraphics(); // 생성자
	virtual ~VooDooGraphics(); // 소멸자

	virtual int PreDraw(VDPRIMITIVETYPE primitiveType, UINT startVertex, UINT primitiveCount); // 그리기 전 준비 작업?
	virtual int Draw();
	virtual int PostDraw() { return 0; } // 그리기 후 작업 // 아직 기능 없음

	virtual int Reset() { return 0; } // 디바이스 리셋 // 아직 기능 없음
	virtual int Release();

	virtual void SetDev(IVooDooDevice* pDevice) { m_pDevice = pDevice; } // 디바이스 설정

	// 내부 메서드
	int  _RenderTargetCreate(); // 렌더 타겟 생성
	void _RenderTargetRelease(); // 렌더 타겟 해제 // 백 버퍼도 해제하게 함
	HDC	 _GetDCRT() const { return m_outputMerge.hDC; } // 현재 렌더 타겟의 DC를 가져옴

	int _SetVertexBuffer(IVooDooVertexBuffer* pVB, UINT Stride); // 정점 버퍼 설정
	int _SetFVF(DWORD fvf); // FVF 설정

	void _TempBufferRelease(); // 아직 기능 없음

	// 장면 관리
	virtual int _BeginScene(); // 씬 시작 // 배경 색, 펜 색 설정
	virtual int _EndScene(); // 씬 끝내기 // 아직 기능 없음
	virtual int _Clear(COLORREF col); // 화면 클리어
	virtual int _Present(); // 현재 씬을 화면에 표시

	// 파이프라인 상태 설정
	virtual int _VertexStreaming(_Out_ VDVECTOR oPos[3], _Out_ VDCOLOR oColor[3]);
	virtual int _VertexPipeline(_In_ VDVECTOR iPos[3], _Out_ VDVECTOR oPos[3], _Inout_ VDCOLOR oColor[3]);
	virtual int _GeometryPipeline(_In_ VDVECTOR iPos[3], _Out_ VDVECTOR oPos[3], _Inout_ VDCOLOR oColor[3]);
	virtual int _PixelPipeline(_In_ VDVECTOR oPos[3], _In_  VDCOLOR oColor[3]);

	virtual int _PrimitiveAssembly(_In_ std::vector<VS_OUTPUT>& vsouts, _Out_ VDVECTOR oPos[3], _Out_ VDCOLOR oColor[3]);
	virtual int _PixelPipeline1(); // 아직 기능 없음
	virtual int _PixelPipeline2(); // 아직 기능 없음

	virtual bool _FaceCulling2(VDVECTOR v0, VDVECTOR v1, VDVECTOR v2);
	virtual bool _FaceCulling3(VDVECTOR v0, VDVECTOR v1, VDVECTOR v2);
	virtual bool _FrustumCulling(VDVECTOR v[3]); // 프러스텀 컬링

	virtual int _HomogeneousDivide(VDVECTOR iPos[3], VDVECTOR oPos[3]);
	virtual int  _Viewport(VDVECTOR iPos[3], VDVECTOR oPos[3]);

	// 고정 함수
	virtual VS_OUTPUT _TnL_VP(VDVECTOR iPos, VDCOLOR iColor);
	virtual int _TnL_GP(_In_ VDVECTOR iPos[3], _Out_ VDVECTOR oPos[3], _Inout_ VDCOLOR oColor[3]);
	virtual int _TnL_PP(_In_ VDVECTOR oPos[3], _In_ VDCOLOR oColor[3]); // 아직 기능 없음

	// 파이프라인 상태 설정
	virtual void _initRenderState();
	virtual int _SetRenderState(VDRENDERSTATETYPE State, DWORD Value) { m_pipelineState.renderState[State] = Value; return 0; } // 렌더 상태 설정
	virtual int _GetRenderState(VDRENDERSTATETYPE State, _Out_ DWORD* pValue) { *pValue = m_pipelineState.renderState[State]; return 0; } // 렌더 상태 가져오기

	// 변환 행렬 설정
	virtual int _SetTransform(VDTRANSFORMSTATETYPE State, const VDMATRIX* pMatrix) { m_transform.transform[State] = *pMatrix; return 0; } // 변환 행렬 설정
	virtual int _GetTransform(VDTRANSFORMSTATETYPE State, _Out_ VDMATRIX* pMatrix) { *pMatrix = m_transform.transform[State]; return 0; } // 변환 행렬 가져오기

	// 뷰포트 관련
	virtual int _SetViewport(VDVIEWPORT* pViewport); // 뷰포트 설정
	virtual int _GetViewport(VDVIEWPORT* pViewport); // 뷰포트 가져오기

	// 그래픽스 API
	virtual int _Line(VDVECTOR& v0, VDVECTOR& v1, VDCOLOR& color);
	virtual int _Line(VDVECTOR& v0, VDVECTOR& v1, VDCOLOR& color1, VDCOLOR& color2);
	virtual int _Face(VDVECTOR pos[3], VDCOLOR color[3]); // 외적 공부 좀 더
	virtual void _HLine(HDC hdc, int x1, int x2, int y, VDCOLOR color); // 수평선 그리기
	virtual void _SetPixel(HDC hdc, int x, int y, VDCOLOR color) { SetPixel(hdc, x, y, color); } // 픽셀 찍기

	virtual void _DrawGrid(); // 그리드 그리기
	virtual void _DrawAxis(); // 축 그리기
};