#include "VDVertexBuffer.h"
#include "VDGUID.h"
#include "VDX9.h"

int VDVertexBuffer::_Create(VDVERTEXBUFFER_DESC desc)
{
	if (m_pVertexBuffer != nullptr) { return -1; } // �̹� ���۰� �����ϸ� ����

	m_pVertexBuffer = static_cast<void*>(new BYTE[desc.Size]); // ���� �޸� �Ҵ�
	assert(m_pVertexBuffer); // �޸� �Ҵ� ���� �� ����
	m_sizeInBytes = desc.Size; // ���� ũ�� ����
	m_FVF = desc.FVF; // FVF ����
	m_desc = desc; // ���� �Ӽ� ����

	m_Stride = 0;
	if (m_FVF & VD2D) m_Stride += sizeof(float) * 2; // XY
	if (m_FVF & VD3D) m_Stride += sizeof(float) * 3; // XYZ
	if (m_FVF & VDDIFFUSE) m_Stride += sizeof(DWORD); // ��ǻ�� ����

	if (m_Stride <= 0) { return -2; } // ��Ʈ���̵尡 0 �����̸� ����

	m_vertexAmount = _GetVertexCount(); // ���� ���� Ȯ��

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
	int result = _Create(desc); // ���� _Create �޼���(VDVERTEXBUFFER_DESC ���) ȣ��

	return result;
}

VDVECTOR VDVertexBuffer::_GetVertexPosition(UINT index) const
{
	BYTE* pCurrentBuffer = static_cast<BYTE*>(m_pVertexBuffer) + (index * m_Stride); // ���� ���� ��ġ ���
	VDVECTOR pos = *reinterpret_cast<VDVECTOR*>(pCurrentBuffer); // ���� ��ġ�� VDVECTOR�� ��ȯ

	return pos;
}

DWORD VDVertexBuffer::_GetDiffuse(UINT index) const
{
	BYTE* pCurrentBuffer = static_cast<BYTE*>(m_pVertexBuffer) + (index * m_Stride); // ���� ���� ��ġ ���
	DWORD color = *reinterpret_cast<DWORD*>(pCurrentBuffer + (m_Stride - sizeof(DWORD))); // ���� ������ DWORD�� ��ȯ

	return color;
}

VDVertexBuffer::VDVertexBuffer()
{
	m_pVertexBuffer = nullptr; // ���� �ʱ�ȭ
	m_sizeInBytes = 0; // ���� ũ�� �ʱ�ȭ
	m_FVF = 0; // FVF �ʱ�ȭ
	m_Stride = 0; // ��Ʈ���̵� ũ�� �ʱ�ȭ
	m_vertexAmount = 0; // ���� ���� �ʱ�ȭ
	m_isLocked = FALSE; // ���� ��� ���� �ʱ�ȭ

	ZeroMemory(&m_desc, sizeof(m_desc)); // ���� �Ӽ� ����ü �ʱ�ȭ
}

VDVertexBuffer::~VDVertexBuffer()
{
	if (m_pVertexBuffer != nullptr)
	{
		delete[] static_cast<BYTE*>(m_pVertexBuffer); // ���� �޸� ����
		m_pVertexBuffer = nullptr; // ������ �ʱ�ȭ
	}
	m_sizeInBytes = 0; // ���� ũ�� �ʱ�ȭ
	m_FVF = 0; // FVF �ʱ�ȭ
	m_Stride = 0; // ��Ʈ���̵� ũ�� �ʱ�ȭ
	m_vertexAmount = 0; // ���� ���� �ʱ�ȭ
	m_isLocked = FALSE; // ���� ��� ���� �ʱ�ȭ
	ZeroMemory(&m_desc, sizeof(m_desc)); // ���� �Ӽ� ����ü �ʱ�ȭ
}

int VDVertexBuffer::Lock(UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags)
{
	if (m_isLocked) { return -1; } // �̹� ��� ������ ����

	BYTE* pCurrentBuffer = static_cast<BYTE*>(m_pVertexBuffer) + OffsetToLock; // ���� ���� ��ġ ���
	*ppbData = static_cast<void*>(pCurrentBuffer); // ���� ������ ������ ����

	m_isLocked = TRUE; // ���� ��� ���� ����

	return 0;
}

int VDVertexBuffer::Unlock()
{
	if (!m_isLocked) { return -1; } // ��� ���� ������ ����

	m_vertexAmount = m_sizeInBytes / m_Stride; // ���� ���� ����
	m_isLocked = FALSE; // ���� ��� ����

	return 0;
}

int VDVertexBuffer::GetDesc(_Out_ VDVERTEXBUFFER_DESC* pDesc)
{
	if (pDesc == nullptr) { return -1; } // pDesc�� nullptr�̸� ����

	*pDesc = m_desc; // ���� �Ӽ� ����ü�� pDesc�� ����

	return 0;
}

int VDVertexBuffer::GetPrivateData(_Out_ void** ppBuffer, _Out_ UINT* pSize)
{
	void* pVertexBuffer = GetBuffer(); // ���۸� ������
	UINT size = GetVertexAmount(); // ���� ������ ������

	*ppBuffer = pVertexBuffer; // ppBuffer�� ���� ������ ����
	*pSize = size; // pSize�� ���� ���� ����

	return 0;
}

int VDVertexBuffer::QueryInterface(VDIID rVDIID, _Out_ void** ppvObject)
{
	if (VDIsEqualVDIID(rVDIID, VDIID_VDIUnknown) || VDIsEqualVDIID(rVDIID, VDIID_IVooDoo))
	{
		AddRef(); // ���� ī��Ʈ ����
		*ppvObject = dynamic_cast<IVooDooVertexBuffer*>(this); // IVooDooVertexBuffer �������̽��� ĳ����
		return 0; // ���������� �������̽��� ��ȯ
	}
	*ppvObject = nullptr;
	return -1;
}