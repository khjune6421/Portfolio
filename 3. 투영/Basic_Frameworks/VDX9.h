#pragma once
#include "VDUnknownbase.h"
#include "VDX9types.h"
#include "VDMath.h"

__interface IVooDoo; // VooDoo �������̽� ���� ����
__interface IVooDooDevice; // VooDooDevice �������̽� ���� ����
__interface IVooDooVertexBuffer; // VooDooVertexBuffer �������̽� ���� ����

// VooDoo �������̽� ����
__interface IVooDooDevice : public VDIUnknownBase
{
	virtual int BeginScene() = 0; // ���� �����ϴ� �޼���
	virtual int EndScene() = 0; // ���� ������ �޼���
	virtual int Clear(COLORREF col) = 0; // ȭ���� ����� �޼���
	virtual int Present() = 0; // ���� ���� ȭ�鿡 ǥ���ϴ� �޼���

	virtual int CreateVertexBuffer(UINT Size, DWORD Usage, DWORD FVF, VDPOOL Pool, _Out_ IVooDooVertexBuffer** ppVertexBuffer, _In_opt_ HANDLE* pSharedHandle) = 0; // ���ؽ� ���۸� �����ϴ� �޼���
	virtual int SetStreamSource(UINT StreamNumber, IVooDooVertexBuffer* pStreamData, UINT OffsetInBytes, UINT Stride) = 0; // ��Ʈ�� �ҽ��� �����ϴ� �޼��� // ���� �𸣰���
	virtual int SetFVF(DWORD FVF) = 0; // FVF�� �����ϴ� �޼���
	virtual int DrawPrimitive(VDPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) = 0; // ������ ���¸� �׸��� �޼���

	virtual int SetRenderState(VDRENDERSTATETYPE State, DWORD Value) = 0; // ���� ���¸� �����ϴ� �޼���
	virtual int GetRenderState(VDRENDERSTATETYPE State, _Out_ DWORD* pValue) = 0; // ���� ���¸� �������� �޼���

	virtual int SetTransform(VDTRANSFORMSTATETYPE State, const VDMATRIX* pMatrix) = 0; // ��ȯ ����� �����ϴ� �޼���
	virtual int GetTransform(VDTRANSFORMSTATETYPE State, _Out_ VDMATRIX* pMatrix) = 0; // ��ȯ ����� �������� �޼���

	virtual HDC GetRT() = 0; // ���� ���� Ÿ���� DC�� �������� �޼���
	virtual COLORREF GetClearColor() = 0; // ���� Ŭ���� ������ �������� �޼���

	virtual void DrawGrid() = 0; // �׸��� �׸��� �޼���
	virtual void DrawAxis() = 0; // �� �׸��� �޼���
};

// VooDooDevice �������̽� ����
typedef __interface IVooDooVertexBuffer : public VDIUnknownBase
{
	virtual int Lock(UINT OffsetToLock, UINT SizeToLock, _Out_ void** ppbData, DWORD Flags) = 0; // ���۸� ��״� �޼���
	virtual int Unlock() = 0; // ������ ����� �����ϴ� �޼���
	virtual int GetDesc(_Out_ VDVERTEXBUFFER_DESC* pDesc) = 0; // ������ �Ӽ��� �������� �޼���(Size, Usage, Pool)

	virtual void* GetBuffer() = 0; // ���۸� �������� �޼���
	virtual UINT GetVertexAmount() = 0; // ������ ���� ������ �������� �޼���
	virtual int GetPrivateData(_Out_ void** ppBuffer, _Out_ UINT* pSize) = 0; // ���������� �����̺� �����͸� �������� �޼��� // ��� ������, �� ����ϴ��� �𸣰���
};

typedef __interface IVooDoo : public VDIUnknownBase
{
		virtual int CreateDevice(HWND hWnd, VDPRESENT_PARAMETERS* pp, DWORD vp, _Out_ IVooDooDevice** ppDevice) = 0; // ����̽��� �����ϴ� �޼���
};

// VooDoo ��ü�� �����ϴ� �Լ� // �� VooDoo.h�� �������� �ʰ� ���� �и��ؾ� �ϴ����� �𸣰���
IVooDoo* VooDooCreate(DWORD ver);