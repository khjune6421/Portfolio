#pragma once
#include "VDGUID.h"
#include "VDUnknownbase.h"
#include "VDX9.h"
#include "VDX9types.h"
#include "VDMath.h"
#include "VDGraphics.h"

class VooDooDevice : public IVooDooDevice // VooDooDevice �������̽� ����
{
	friend class VooDoo; // VooDoo Ŭ�������� VooDooDevice�� �����ڸ� ȣ���� �� �ֵ���

protected:
	ULONG m_refCount = 1; // ���� ī��Ʈ

	HWND m_hWnd; // ������ �ڵ�
	DWORD m_vertexProcessing; // ����̽� ���� �÷���

	HDC m_hRT; // ����̽� ���ؽ�Ʈ �ڵ�
	COLORREF m_clearColor; // Ŭ���� ����

	VooDooGraphics* m_pGraphics; // VooDooGraphics ������

	int _Create(HWND hWnd, VDPRESENT_PARAMETERS* pp, DWORD vp); // ����̽� ���� �Լ�

public:
	VooDooDevice();
	virtual ~VooDooDevice();

	virtual int BeginScene() override { m_pGraphics->_BeginScene(); return 0; }; // �� ���� // m_pGraphics�� BeginScene ȣ��
	virtual int EndScene() override { m_pGraphics->_EndScene(); return 0; };
	virtual int Clear(COLORREF col) override { m_clearColor = col; return m_pGraphics->_Clear(col); };
	virtual int Present() override { return m_pGraphics->_Present(); };

	virtual int CreateVertexBuffer(UINT Size, DWORD Usage, DWORD FVF, VDPOOL Pool, _Out_ IVooDooVertexBuffer** ppVertexBuffer, _In_opt_ HANDLE* pSharedHandle) override;
	virtual int SetStreamSource(UINT StreamNumber, IVooDooVertexBuffer* pStreamData, UINT OffsetInBytes, UINT Stride) override;
	virtual int SetFVF(DWORD FVF) override { return m_pGraphics->_SetFVF(FVF); }; // FVF ����
	virtual int DrawPrimitive(VDPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) override;

	virtual int SetRenderState(VDRENDERSTATETYPE State, DWORD Value) override { return m_pGraphics->_SetRenderState(State, Value); }; // ���� ���� ����
	virtual int GetRenderState(VDRENDERSTATETYPE State, _Out_ DWORD* pValue) override { return m_pGraphics->_GetRenderState(State, pValue); }; // ���� ���� ��������

	virtual int SetTransform(VDTRANSFORMSTATETYPE State, const VDMATRIX* pMatrix) override { return m_pGraphics->_SetTransform(State, pMatrix); }; // ��ȯ ��� ����
	virtual int GetTransform(VDTRANSFORMSTATETYPE State, _Out_ VDMATRIX* pMatrix) override { return m_pGraphics->_GetTransform(State, pMatrix); }; // ��ȯ ��� ��������

	virtual int SetViewport(VDVIEWPORT* pViewport) override { return m_pGraphics->_SetViewport(pViewport); }; // ����Ʈ ����
	virtual int GetViewport(VDVIEWPORT* pViewport) override { return m_pGraphics->_GetViewport(pViewport); }; // ����Ʈ ��������

	virtual HDC GetRT() override { return m_pGraphics->_GetDCRT(); }; // ���� ���� Ÿ���� DC ��������
	virtual COLORREF GetClearColor() override { return m_clearColor; }; // ���� Ŭ���� ���� ��������

	virtual void DrawGrid() override { m_pGraphics->_DrawGrid(); }; // �׸��� �׸���
	virtual void DrawAxis() { m_pGraphics->_DrawAxis(); }; // �� �׸���

	virtual ULONG AddRef() override { return ++m_refCount; }; // ���� ī��Ʈ ����
	virtual ULONG Release() override { if (--m_refCount == 0) { delete this; return 0; } return m_refCount; } // ���� ī��Ʈ ���� �� ��ü ����
	virtual int QueryInterface(VDIID rVDIID, void** ppvObject) override;
};

class VooDoo : public IVooDoo // VooDoo �������̽� ����
{
protected:
	ULONG m_refCount = 1; // ���� ī��Ʈ

public:
	VooDoo(); // ������ // ���� ��� ����
	virtual ~VooDoo(); // �Ҹ��� // ���������� ���� ��� ����

	virtual int CreateDevice(HWND hWnd, VDPRESENT_PARAMETERS* pp, DWORD vp, _Out_ IVooDooDevice** ppDevice);

	virtual ULONG AddRef() override { return ++m_refCount; }; // ���� ī��Ʈ ����
	virtual ULONG Release() override { if (--m_refCount == 0) { delete this; return 0; } return m_refCount; } // ���� ī��Ʈ ���� �� ��ü ����
	virtual int QueryInterface(VDIID rVDIID, _Out_ void** ppvObject) override;
};