#include "VDVertexBuffer.h"
#include "VDGUID.h"
#include "VDX9.h"

int VDVertexBuffer::_Create(VDVERTEXBUFFER_DESC desc)
{
	if (m_pVertexBuffer != nullptr) { return -1; } // 이미 버퍼가 존재하면 에러

	m_pVertexBuffer = static_cast<void*>(new BYTE[desc.Size]); // 버퍼 메모리 할당
	assert(m_pVertexBuffer); // 메모리 할당 실패 시 에러
	m_sizeInBytes = desc.Size; // 버퍼 크기 설정
	m_FVF = desc.FVF; // FVF 설정
	m_desc = desc; // 버퍼 속성 설정

	m_Stride = 0;
	if (m_FVF & VD2D) m_Stride += sizeof(float) * 2; // XY
	if (m_FVF & VD3D) m_Stride += sizeof(float) * 3; // XYZ
	if (m_FVF & VDDIFFUSE) m_Stride += sizeof(DWORD); // 디퓨즈 색상

	if (m_Stride <= 0) { return -2; } // 스트라이드가 0 이하이면 에러

	m_vertexAmount = _GetVertexCount(); // 정점 개수 확인

    return 0;
}

int VDVertexBuffer::_Create(UINT length, DWORD FVF, VDPOOL Pool)
{
	VDVERTEXBUFFER_DESC desc = {};
	desc.Format = VDFORMAT::VDFMT_VERTEXDATA;
	desc.Type = VDRTYPE_VERTEXBUFFER;
	desc.Usage = VDUSAGE_WRITEONLY;
	desc.Pool = Pool;
	desc.Size = length;
	desc.FVF = FVF;
	int result = _Create(desc); // 위의 _Create 메서드(VDVERTEXBUFFER_DESC 사용) 호출

	return result;
}

VDVECTOR VDVertexBuffer::_GetVertexPosition(UINT index) const
{
	BYTE* pCurrentBuffer = static_cast<BYTE*>(m_pVertexBuffer) + (index * m_Stride); // 현재 정점 위치 계산
	VDVECTOR pos = *reinterpret_cast<VDVECTOR*>(pCurrentBuffer); // 정점 위치를 VDVECTOR로 변환

	return pos;
}

DWORD VDVertexBuffer::_GetDiffuse(UINT index) const
{
	BYTE* pCurrentBuffer = static_cast<BYTE*>(m_pVertexBuffer) + (index * m_Stride); // 현재 정점 위치 계산
	DWORD color = *reinterpret_cast<DWORD*>(pCurrentBuffer + (m_Stride - sizeof(DWORD))); // 정점 색상을 DWORD로 변환

	return color;
}

VDVertexBuffer::VDVertexBuffer()
{
	m_pVertexBuffer = nullptr; // 버퍼 초기화
	m_sizeInBytes = 0; // 버퍼 크기 초기화
	m_FVF = 0; // FVF 초기화
	m_Stride = 0; // 스트라이드 크기 초기화
	m_vertexAmount = 0; // 정점 개수 초기화
	m_isLocked = FALSE; // 버퍼 잠금 상태 초기화

	ZeroMemory(&m_desc, sizeof(m_desc)); // 버퍼 속성 구조체 초기화
}

VDVertexBuffer::~VDVertexBuffer()
{
	if (m_pVertexBuffer != nullptr)
	{
		delete[] static_cast<BYTE*>(m_pVertexBuffer); // 버퍼 메모리 해제
		m_pVertexBuffer = nullptr; // 포인터 초기화
	}
	m_sizeInBytes = 0; // 버퍼 크기 초기화
	m_FVF = 0; // FVF 초기화
	m_Stride = 0; // 스트라이드 크기 초기화
	m_vertexAmount = 0; // 정점 개수 초기화
	m_isLocked = FALSE; // 버퍼 잠금 상태 초기화
	ZeroMemory(&m_desc, sizeof(m_desc)); // 버퍼 속성 구조체 초기화
}

int VDVertexBuffer::Lock(UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags)
{
	if (m_isLocked) { return -1; } // 이미 잠겨 있으면 에러

	BYTE* pCurrentBuffer = static_cast<BYTE*>(m_pVertexBuffer) + OffsetToLock; // 현재 버퍼 위치 계산
	*ppbData = static_cast<void*>(pCurrentBuffer); // 버퍼 데이터 포인터 설정

	m_isLocked = TRUE; // 버퍼 잠금 상태 설정

	return 0;
}

int VDVertexBuffer::Unlock()
{
	if (!m_isLocked) { return -1; } // 잠겨 있지 않으면 에러

	m_vertexAmount = m_sizeInBytes / m_Stride; // 정점 개수 재계산
	m_isLocked = FALSE; // 버퍼 잠금 해제

	return 0;
}

int VDVertexBuffer::GetDesc(_Out_ VDVERTEXBUFFER_DESC* pDesc)
{
	if (pDesc == nullptr) { return -1; } // pDesc가 nullptr이면 에러

	*pDesc = m_desc; // 버퍼 속성 구조체를 pDesc에 복사

	return 0;
}

int VDVertexBuffer::GetPrivateData(_Out_ void** ppBuffer, _Out_ UINT* pSize)
{
	void* pVertexBuffer = GetBuffer(); // 버퍼를 가져옴
	UINT size = GetVertexAmount(); // 정점 개수를 가져옴

	*ppBuffer = pVertexBuffer; // ppBuffer에 버퍼 포인터 설정
	*pSize = size; // pSize에 정점 개수 설정

	return 0;
}

int VDVertexBuffer::QueryInterface(VDIID rVDIID, _Out_ void** ppvObject)
{
	if (VDIsEqualVDIID(rVDIID, VDIID_VDIUnknown) || VDIsEqualVDIID(rVDIID, VDIID_IVooDoo))
	{
		AddRef(); // 참조 카운트 증가
		*ppvObject = dynamic_cast<IVooDooVertexBuffer*>(this); // IVooDooVertexBuffer 인터페이스로 캐스팅
		return 0; // 성공적으로 인터페이스를 반환
	}
	*ppvObject = nullptr;
	return -1;
}