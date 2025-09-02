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
	if (hWnd == 0) return -1; // �ڵ� �� ����
	if (pp == 0) return -2; // ������Ʈ �Ķ���� �� ����
	if (ppDevice == 0) return -3; // ����̽� ������ �� ����

	VooDooDevice* pDevice = new VooDooDevice; // VooDooDevice ��ü ����
	assert(pDevice);
	pDevice->_Create(hWnd, pp, vp); // ����̽� ���� �Լ� ȣ��

	IVooDooDevice* pInterface = nullptr;
	if (pDevice->QueryInterface(VDIID_IVooDooDevice, reinterpret_cast<void**>(&pInterface)) != 0) return -4; // �������̽� �������� ����

	*ppDevice = pInterface; // �������̽� ������ ��ȯ
	return 0; // ����
}

int VooDoo::QueryInterface(VDIID rVDIID, void** ppvObject)
{
	if (VDIsEqualVDIID(rVDIID, VDIID_IVooDoo))
	{
		*ppvObject = static_cast<IVooDoo*>(this); // IVooDoo �������̽� ��ȯ
		AddRef(); // ���� ī��Ʈ ����
		return 0; // ����
	}
	return -1; // ����
}

int VooDooDevice::_Create(HWND hWnd, VDPRESENT_PARAMETERS* pp, DWORD vp)
{
	if (hWnd == nullptr) return -1; // �ڵ� �� ����
	if (pp == nullptr) return -2; // ������Ʈ �Ķ���� �� ����

	VooDooGraphics* pGraphics = new VooDooGraphics; // VooDooGraphics ��ü ����
	assert(pGraphics);

	VooDooGraphics::OUTPUT_MERGE outputMerge = {};
	outputMerge.hWnd = hWnd; // ��� ������ �ڵ� ����
	outputMerge.presentParameters = *pp; // ������Ʈ �Ķ���� ����
	pGraphics->m_outputMerge = outputMerge; // ��� ���� ����ü ����
	pGraphics->_RenderTargetCreate(); // ���� Ÿ�� ����

	pGraphics->SetDev(this);

	m_hWnd = hWnd; // ������ �ڵ� ����
	m_vertexProcessing = vp; // ����̽� ���� �÷��� ����
	m_pGraphics = pGraphics; // VooDooGraphics ������ ����
	m_hRT = pGraphics->_GetDCRT(); // ����̽� ���ؽ�Ʈ �ڵ� ����
	m_clearColor = VDCOLOR(0.25f, 0.25f, 0.25f, 1.0f); // ���� ����

	return 0;
}

VooDooDevice::VooDooDevice()
{
	m_hWnd = nullptr; // ������ �ڵ� �ʱ�ȭ
	m_vertexProcessing = 0; // ����̽� ���� �÷��� �ʱ�ȭ

	m_hRT = nullptr; // ����̽� ���ؽ�Ʈ �ڵ� �ʱ�ȭ
	m_clearColor = VDCOLOR(0.25f, 0.25f, 0.25f, 1.0f); // ���� �ʱ�ȭ

	m_pGraphics = nullptr; // VooDooGraphics ������ �ʱ�ȭ
}

VooDooDevice::~VooDooDevice()
{
	if (m_pGraphics)
	{
		m_pGraphics->_RenderTargetRelease(); // ���� Ÿ�� ����
		delete m_pGraphics; // VooDooGraphics ��ü ����
		m_pGraphics = nullptr; // ������ �ʱ�ȭ
	}
	if (m_hRT)
	{
		DeleteDC(m_hRT); // ����̽� ���ؽ�Ʈ �ڵ� ����
		m_hRT = nullptr; // ������ �ʱ�ȭ
	}
}

int VooDooDevice::CreateVertexBuffer(UINT Size, DWORD Usage, DWORD FVF, VDPOOL Pool, IVooDooVertexBuffer** ppVertexBuffer, _In_opt_ HANDLE* pSharedHandle)
{
	VDVertexBuffer* pVB = new VDVertexBuffer; // VDVertexBuffer ��ü ����
	if (!pVB) return -1;

	VDVERTEXBUFFER_DESC desc = {};
	desc.Format = VDFMT_VERTEXDATA;
	desc.Type = VDRTYPE_VERTEXBUFFER;
	desc.Usage = VDUSAGE_WRITEONLY;
	desc.Pool = Pool; // Ǯ ����
	desc.Size = Size; // ���� ũ�� ����
	desc.FVF = FVF; // FVF ����

	if (pVB->_Create(desc) != 0) return -2;

	IVooDooVertexBuffer* pInterface = nullptr;
	if (pVB->QueryInterface(VDIID_IVooDooVertexBuffer, reinterpret_cast<void**>(&pInterface)) != 0) return -3; // �������̽� �������� ����

	*ppVertexBuffer = pInterface; // �������̽� ������ ��ȯ

	return 0;
}

int VooDooDevice::SetStreamSource(UINT StreamNumber, IVooDooVertexBuffer* pStreamData, UINT OffsetInBytes, UINT Stride)
{
	if (pStreamData == nullptr) return -1; // ��Ʈ�� �����Ͱ� nullptr�̸� ���� ��ȯ

	m_pGraphics->_SetVertexBuffer(pStreamData, Stride); // VooDooGraphics�� SetVertexBuffer ȣ��
	return 0;
}

int VooDooDevice::DrawPrimitive(VDPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
	if (m_pGraphics->m_inputSystem.pVertexBuffer == 0) { return -1; } // ���ؽ� ���۰� �������� �ʾ����� ���� ��ȯ
	if (m_pGraphics->m_inputSystem.Stride == 0) { return -2; } // ��Ʈ���̵尡 �������� �ʾ����� ���� ��ȯ

	m_pGraphics->PreDraw(PrimitiveType, StartVertex, PrimitiveCount); // �׸��� �� ó��
	m_pGraphics->Draw(); // �׸��� ����
	m_pGraphics->PostDraw(); // �׸��� �� ó��
}

int VooDooDevice::QueryInterface(VDIID rVDIID, _Out_ void** ppvObject)
{
	if (VDIsEqualVDIID(rVDIID, VDIID_IVooDooDevice))
	{
		*ppvObject = static_cast<IVooDooDevice*>(this); // IVooDooDevice �������̽� ��ȯ
		AddRef(); // ���� ī��Ʈ ����
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

	return pInterface; // ���������� �������̽� ��ȯ
}