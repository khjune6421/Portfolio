#pragma once
#include <vector>
#include "VDMath.h"
#include "VDX9types.h"
#include "VDX9.h"

struct VS_OUTPUT
{
	VDVECTOR Pos; // ��ġ
	VDCOLOR Color; // ����
};

typedef std::vector<VS_OUTPUT> VS_OUTS; // ���� ó�� ��� ���� ����

class VooDooCore
{
	friend class VooDooDevice;

public:
	VooDooCore(); // ������ // ���� ��� ����
	virtual ~VooDooCore(); // �Ҹ��� // ���������� ���� ��� ����
};

class VooDooGraphics : public VooDooCore
{
	HPEN m_hPen = nullptr;
	HPEN m_hOldPen = nullptr;

	friend class VooDooDevice;

public:
	struct INPUT_SYSTEM
	{
		IVooDooVertexBuffer* pVertexBuffer[1]; // �Է� ����
		DWORD FVF;
		UINT Stride;

		VDPRIMITIVETYPE PrimitiveType;
		UINT PrimitiveCount; // �׸� ������Ƽ�� ����
		UINT StartVertex; // ���� ���� �ε���
		UINT VertexAmount; // ���� ����
	};
	struct STREAM_DBG
	{
		UINT currentVertexIndex; // ���� ���� �ε���
		UINT currentPrimitiveIndex; // ���� ���� ���� �ε���
		UINT currentPrimitiveDrawCount; // ���� ���� �׸��� ����
	};
	struct TRANSFORM
	{
		VDMATRIX transform[4]; // ��ȯ ��� �迭 // 0: World, 1: View, 2: Projection
	};
	struct PIPELINE_STATE
	{
		DWORD renderState[256]; // ���� ���� �迭
	};
	struct OUTPUT_MERGE
	{
		HWND hWnd; // ��� ������ �ڵ�
		VDPRESENT_PARAMETERS presentParameters; // ��� ����(�ػ�, �� ���� ũ�� ��)
		HBITMAP hBackBuffer; // �� ���� �ڵ�
		HDC hDC; // ����̽� ���ؽ�Ʈ �ڵ�
		VDVIEWPORT m_viewport; // ����Ʈ ����ü
		HRGN m_hRGN; // ���� ���� Ÿ���� ���� �ڵ�
	};
	struct DEPTH_BUFFER
	{
		std::vector<float> depthData; // ���� ���� ������
		UINT depthWidth; // ���� ���� �ʺ�
		UINT depthHeight; // ���� ���� ����
	};

protected:
	IVooDooDevice* m_pDevice; // VooDoo ����̽� ������
	INPUT_SYSTEM m_inputSystem; // �Է� �ý��� ����ü
	STREAM_DBG m_streamDbg; // ��Ʈ�� ����� ����ü
	TRANSFORM m_transform; // ��ȯ ��� ����ü
	PIPELINE_STATE m_pipelineState; // ���������� ���� ����ü
	OUTPUT_MERGE m_outputMerge; // ��� ���� ����ü
	DEPTH_BUFFER m_depthBuffer; // ���� ���� ����ü

public:
	VooDooGraphics(); // ������
	virtual ~VooDooGraphics(); // �Ҹ���

	virtual int PreDraw(VDPRIMITIVETYPE primitiveType, UINT startVertex, UINT primitiveCount); // �׸��� �� �غ� �۾�?
	virtual int Draw();
	virtual int PostDraw() { return 0; } // �׸��� �� �۾� // ���� ��� ����

	virtual int Reset() { return 0; } // ����̽� ���� // ���� ��� ����
	virtual int Release();

	virtual void SetDev(IVooDooDevice* pDevice) { m_pDevice = pDevice; } // ����̽� ����

	// ���� �޼���
	int  _RenderTargetCreate(); // ���� Ÿ�� ����
	void _RenderTargetRelease(); // ���� Ÿ�� ���� // �� ���۵� �����ϰ� ��
	HDC	 _GetDCRT() const { return m_outputMerge.hDC; } // ���� ���� Ÿ���� DC�� ������

	int _SetVertexBuffer(IVooDooVertexBuffer* pVB, UINT Stride); // ���� ���� ����
	int _SetFVF(DWORD fvf); // FVF ����

	void _TempBufferRelease(); // ���� ��� ����

	// ��� ����
	virtual int _BeginScene(); // �� ���� // ��� ��, �� �� ����
	virtual int _EndScene(); // �� ������ // ���� ��� ����
	virtual int _Clear(COLORREF col); // ȭ�� Ŭ����
	virtual int _Present(); // ���� ���� ȭ�鿡 ǥ��

	// ���������� ���� ����
	virtual int _VertexStreaming(_Out_ VDVECTOR oPos[3], _Out_ VDCOLOR oColor[3]);
	virtual int _VertexPipeline(_In_ VDVECTOR iPos[3], _Out_ VDVECTOR oPos[3], _Inout_ VDCOLOR oColor[3]);
	virtual int _GeometryPipeline(_In_ VDVECTOR iPos[3], _Out_ VDVECTOR oPos[3], _Inout_ VDCOLOR oColor[3]);
	virtual int _PixelPipeline(_In_ VDVECTOR oPos[3], _In_  VDCOLOR oColor[3]);

	virtual int _PrimitiveAssembly(_In_ std::vector<VS_OUTPUT>& vsouts, _Out_ VDVECTOR oPos[3], _Out_ VDCOLOR oColor[3]);
	virtual int _PixelPipeline1(); // ���� ��� ����
	virtual int _PixelPipeline2(); // ���� ��� ����

	virtual bool _FaceCulling2(VDVECTOR v0, VDVECTOR v1, VDVECTOR v2);
	virtual bool _FaceCulling3(VDVECTOR v0, VDVECTOR v1, VDVECTOR v2);
	virtual bool _FrustumCulling(VDVECTOR v[3]); // �������� �ø�

	virtual int _HomogeneousDivide(VDVECTOR iPos[3], VDVECTOR oPos[3]);
	virtual int  _Viewport(VDVECTOR iPos[3], VDVECTOR oPos[3]);

	// ���� �Լ�
	virtual VS_OUTPUT _TnL_VP(VDVECTOR iPos, VDCOLOR iColor);
	virtual int _TnL_GP(_In_ VDVECTOR iPos[3], _Out_ VDVECTOR oPos[3], _Inout_ VDCOLOR oColor[3]);
	virtual int _TnL_PP(_In_ VDVECTOR oPos[3], _In_ VDCOLOR oColor[3]); // ���� ��� ����

	// ���������� ���� ����
	virtual void _initRenderState();
	virtual int _SetRenderState(VDRENDERSTATETYPE State, DWORD Value) { m_pipelineState.renderState[State] = Value; return 0; } // ���� ���� ����
	virtual int _GetRenderState(VDRENDERSTATETYPE State, _Out_ DWORD* pValue) { *pValue = m_pipelineState.renderState[State]; return 0; } // ���� ���� ��������

	// ��ȯ ��� ����
	virtual int _SetTransform(VDTRANSFORMSTATETYPE State, const VDMATRIX* pMatrix) { m_transform.transform[State] = *pMatrix; return 0; } // ��ȯ ��� ����
	virtual int _GetTransform(VDTRANSFORMSTATETYPE State, _Out_ VDMATRIX* pMatrix) { *pMatrix = m_transform.transform[State]; return 0; } // ��ȯ ��� ��������

	// ����Ʈ ����
	virtual int _SetViewport(VDVIEWPORT* pViewport); // ����Ʈ ����
	virtual int _GetViewport(VDVIEWPORT* pViewport); // ����Ʈ ��������

	// �׷��Ƚ� API
	virtual int _Line(VDVECTOR& v0, VDVECTOR& v1, VDCOLOR& color);
	virtual int _Line(VDVECTOR& v0, VDVECTOR& v1, VDCOLOR& color1, VDCOLOR& color2);
	virtual int _Face(VDVECTOR pos[3], VDCOLOR color[3]); // ���� ���� �� ��
	virtual void _HLine(HDC hdc, int x1, int x2, int y, VDCOLOR color); // ���� �׸���
	virtual void _SetPixel(HDC hdc, int x, int y, VDCOLOR color) { SetPixel(hdc, x, y, color); } // �ȼ� ���

	virtual void _DrawGrid(); // �׸��� �׸���
	virtual void _DrawAxis(); // �� �׸���
};