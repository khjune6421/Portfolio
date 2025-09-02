#include "VooDoo.h"
#include "VDVertexBuffer.h"
#include "VDGUID.h"

VooDoo::VooDoo()
{
}

VooDoo::~VooDoo()
{
}

int VooDoo::CreateDevice(HWND hWnd, VDPRESENT_PARAMETERS* pp, DWORD vp, IVooDooDevice** ppDevice)
{
	if (hWnd == 0) return -1; // 핸들 값 오류
	if (pp == 0) return -2; // 프레젠트 파라미터 값 오류
	if (ppDevice == 0) return -3; // 디바이스 포인터 값 오류

	VooDooDevice* pDevice = new VooDooDevice; // VooDooDevice 객체 생성
	assert(pDevice);
	pDevice->_Create(hWnd, pp, vp); // 디바이스 생성 함수 호출

	IVooDooDevice* pInterface = nullptr;
	if (pDevice->QueryInterface(VDIID_IVooDooDevice, reinterpret_cast<void**>(&pInterface)) != 0) return -4; // 인터페이스 가져오기 실패

	*ppDevice = pInterface; // 인터페이스 포인터 반환
	return 0; // 성공
}

int VooDoo::QueryInterface(VDIID rVDIID, void** ppvObject)
{
	if (VDIsEqualVDIID(rVDIID, VDIID_IVooDoo))
	{
		*ppvObject = static_cast<IVooDoo*>(this); // IVooDoo 인터페이스 반환
		AddRef(); // 참조 카운트 증가
		return 0; // 성공
	}
	return -1; // 실패
}

int VooDooDevice::_Create(HWND hWnd, VDPRESENT_PARAMETERS* pp, DWORD vp)
{
	if (hWnd == nullptr) return -1; // 핸들 값 오류
	if (pp == nullptr) return -2; // 프레젠트 파라미터 값 오류

	VooDooGraphics* pGraphics = new VooDooGraphics; // VooDooGraphics 객체 생성
	assert(pGraphics);

	VooDooGraphics::OUTPUT_MERGE outputMerge = {};
	outputMerge.hWnd = hWnd; // 출력 윈도우 핸들 설정
	outputMerge.presentParameters = *pp; // 프레젠트 파라미터 설정
	pGraphics->m_outputMerge = outputMerge; // 출력 병합 구조체 설정
	pGraphics->_RenderTargetCreate(); // 렌더 타겟 생성

	pGraphics->SetDev(this);

	m_hWnd = hWnd; // 윈도우 핸들 설정
	m_vertexProcessing = vp; // 디바이스 생성 플래그 설정
	m_pGraphics = pGraphics; // VooDooGraphics 포인터 설정
	m_hRT = pGraphics->_GetDCRT(); // 디바이스 컨텍스트 핸들 설정
	m_clearColor = VDCOLOR(0.25f, 0.25f, 0.25f, 1.0f); // 배경색 설정

	return 0;
}

VooDooDevice::VooDooDevice()
{
	m_hWnd = nullptr; // 윈도우 핸들 초기화
	m_vertexProcessing = 0; // 디바이스 생성 플래그 초기화

	m_hRT = nullptr; // 디바이스 컨텍스트 핸들 초기화
	m_clearColor = VDCOLOR(0.25f, 0.25f, 0.25f, 1.0f); // 배경색 초기화

	m_pGraphics = nullptr; // VooDooGraphics 포인터 초기화
}

VooDooDevice::~VooDooDevice()
{
	if (m_pGraphics)
	{
		m_pGraphics->_RenderTargetRelease(); // 렌더 타겟 해제
		delete m_pGraphics; // VooDooGraphics 객체 삭제
		m_pGraphics = nullptr; // 포인터 초기화
	}
	if (m_hRT)
	{
		DeleteDC(m_hRT); // 디바이스 컨텍스트 핸들 해제
		m_hRT = nullptr; // 포인터 초기화
	}
}

int VooDooDevice::CreateVertexBuffer(UINT Size, DWORD Usage, DWORD FVF, VDPOOL Pool, IVooDooVertexBuffer** ppVertexBuffer, _In_opt_ HANDLE* pSharedHandle)
{
	VDVertexBuffer* pVB = new VDVertexBuffer; // VDVertexBuffer 객체 생성
	if (!pVB) return -1;

	VDVERTEXBUFFER_DESC desc = {};
	desc.Format = VDFMT_VERTEXDATA;
	desc.Type = VDRTYPE_VERTEXBUFFER;
	desc.Usage = VDUSAGE_WRITEONLY;
	desc.Pool = Pool; // 풀 설정
	desc.Size = Size; // 버퍼 크기 설정
	desc.FVF = FVF; // FVF 설정

	if (pVB->_Create(desc) != 0) return -2;

	IVooDooVertexBuffer* pInterface = nullptr;
	if (pVB->QueryInterface(VDIID_IVooDooVertexBuffer, reinterpret_cast<void**>(&pInterface)) != 0) return -3; // 인터페이스 가져오기 실패

	*ppVertexBuffer = pInterface; // 인터페이스 포인터 반환

	return 0;
}

int VooDooDevice::SetStreamSource(UINT StreamNumber, IVooDooVertexBuffer* pStreamData, UINT OffsetInBytes, UINT Stride)
{
	if (pStreamData == nullptr) return -1; // 스트림 데이터가 nullptr이면 오류 반환

	m_pGraphics->_SetVertexBuffer(pStreamData, Stride); // VooDooGraphics의 SetVertexBuffer 호출
	return 0;
}

int VooDooDevice::DrawPrimitive(VDPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
	if (m_pGraphics->m_inputSystem.pVertexBuffer == 0) { return -1; } // 버텍스 버퍼가 설정되지 않았으면 오류 반환
	if (m_pGraphics->m_inputSystem.Stride == 0) { return -2; } // 스트라이드가 설정되지 않았으면 오류 반환

	m_pGraphics->PreDraw(PrimitiveType, StartVertex, PrimitiveCount); // 그리기 후 처리
	m_pGraphics->Draw(); // 그리기 수행
	m_pGraphics->PostDraw(); // 그리기 후 처리
}

int VooDooDevice::QueryInterface(VDIID rVDIID, _Out_ void** ppvObject)
{
	if (VDIsEqualVDIID(rVDIID, VDIID_IVooDooDevice))
	{
		*ppvObject = static_cast<IVooDooDevice*>(this); // IVooDooDevice 인터페이스 반환
		AddRef(); // 참조 카운트 증가
	}

	return 0;
}

IVooDoo* VooDooCreate(DWORD ver)
{
	VooDoo* pVooDoo = new VooDoo;
	assert(pVooDoo);

	IVooDoo* pInterface = nullptr;
	if (pVooDoo->QueryInterface(VDIID_IVooDoo, reinterpret_cast<void**>(&pInterface)) != 0)
	{
	}

	return pInterface; // 성공적으로 인터페이스 반환
}