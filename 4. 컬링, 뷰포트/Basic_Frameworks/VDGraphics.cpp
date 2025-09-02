#include "VDGraphics.h"
#include "VDVertexBuffer.h"

VooDooCore::VooDooCore()
{
}

VooDooCore::~VooDooCore()
{
}

VooDooGraphics::VooDooGraphics()
{
	Reset();
}

VooDooGraphics::~VooDooGraphics()
{
	Release();
}

int VooDooGraphics::PreDraw(VDPRIMITIVETYPE primitiveType, UINT startVertex, UINT primitiveCount)
{
	m_inputSystem.PrimitiveType = primitiveType;
	m_inputSystem.StartVertex = startVertex;
	m_inputSystem.PrimitiveCount = primitiveCount;

	switch (m_inputSystem.PrimitiveType)
	{
	case VDPT_LINELIST: m_inputSystem.VertexAmount = 2;
		break;

	default:
	case VDPT_TRIANGLELIST: m_inputSystem.VertexAmount = 3;
		break;
	}
	return 0;
}

int VooDooGraphics::Draw()
{
	if (m_inputSystem.pVertexBuffer[0] == nullptr) { return -1; } // ���� ���۰� ������ ����
	if (m_inputSystem.Stride == 0) { return -2; } // ��Ʈ���̵尡 0�̸� ����

	VDVECTOR vPos[3]; // ���� ��ǥ
	VDVECTOR oPos3[3]; // ���� ��ǥ // ���� vPos�� �ٸ� �ǰ�? // �Ƹ��� ��¿�?
	VDVECTOR oPos4[3]; // 4���� ���� ��ǥ?
	VDCOLOR oColor[3]; // ���� ����

	m_streamDbg.currentVertexIndex = m_inputSystem.StartVertex; // ���� ���� �ε��� �ʱ�ȭ
	m_streamDbg.currentPrimitiveIndex = -1; // ���� ���� ���� �ε��� �ʱ�ȭ
	m_streamDbg.currentPrimitiveDrawCount = 0; // ���� �׸��� �ִ� ���� ���� �ʱ�ȭ

	int result = 0;

	while (true)
	{
		result = _VertexStreaming(vPos, oColor); // ���� ��Ʈ����
		if (result < 0) { return result; } // ���� �߻� �� ��ȯ

		_VertexPipeline(vPos, oPos4, oColor); // ���� ���������� ó��

		result = _GeometryPipeline(oPos4, oPos3, oColor); // ������ ���������� ó��
		if (result < 0) { return result; } // ���� �߻� �� ��ȯ

		_PixelPipeline(oPos3, oColor); // �ȼ� ���������� ó��

		m_streamDbg.currentVertexIndex += m_inputSystem.VertexAmount; // ���� ���� �ε��� ������Ʈ
		m_streamDbg.currentPrimitiveIndex = m_streamDbg.currentVertexIndex / m_inputSystem.VertexAmount; // ���� ���� ���� �ε��� ������Ʈ

		if (++m_streamDbg.currentPrimitiveDrawCount >= m_inputSystem.PrimitiveCount)
		{
			break;
		}
	}

	return 0;
}

int VooDooGraphics::Release()
{
	_RenderTargetRelease(); // ���� Ÿ�� ����

	_TempBufferRelease(); // �ӽ� ���� ���� // ���� ��� ����
	return 0;
}

int VooDooGraphics::_RenderTargetCreate()
{
	HDC hDC = GetDC(m_outputMerge.hWnd);

	m_outputMerge.hDC = CreateCompatibleDC(hDC);
	m_outputMerge.hBackBuffer = static_cast<HBITMAP>(CreateCompatibleBitmap(hDC, m_outputMerge.presentParameters.BackBufferWidth, m_outputMerge.presentParameters.BackBufferHeight));
	
	SelectObject(m_outputMerge.hDC, m_outputMerge.hBackBuffer);
	ReleaseDC(m_outputMerge.hWnd, hDC);

	return 0;
}

void VooDooGraphics::_RenderTargetRelease()
{
	if (m_outputMerge.hDC)
	{
		DeleteDC(m_outputMerge.hDC);
		m_outputMerge.hDC = nullptr;
	}
	if (m_outputMerge.hBackBuffer)
	{
		DeleteObject(m_outputMerge.hBackBuffer);
		m_outputMerge.hBackBuffer = nullptr;
	}
	if (m_outputMerge.m_hRGN)
	{
		DeleteObject(m_outputMerge.m_hRGN);
		m_outputMerge.m_hRGN = nullptr;
	}
}

int VooDooGraphics::_SetVertexBuffer(IVooDooVertexBuffer* pVB, UINT Stride)
{
	if (pVB == nullptr) { return -1; } // ���ؽ� ���۰� ������ ����

	m_inputSystem.pVertexBuffer[0] = pVB; // ���ؽ� ���� ����
	m_inputSystem.Stride = Stride; // ��Ʈ���̵� ����

	return 0;
}

int VooDooGraphics::_SetFVF(DWORD fvf)
{
	m_inputSystem.FVF = fvf; // FVF ����

	return 0;
}

void VooDooGraphics::_TempBufferRelease()
{
}

int VooDooGraphics::_BeginScene()
{
	SetBkMode(m_outputMerge.hDC, TRANSPARENT); // �� ��� ���� // ����

	m_hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255)); // ��� �� ���� // CreatePen ���
	m_hOldPen = (HPEN)SelectObject(m_outputMerge.hDC, m_hPen); // DC�� �� ����

	return 0;
}

int VooDooGraphics::_EndScene()
{
	if (m_hOldPen) {
		SelectObject(m_outputMerge.hDC, m_hOldPen);
		m_hOldPen = nullptr;
	}
	if (m_hPen) {
		DeleteObject(m_hPen);
		m_hPen = nullptr;
	}
	return 0;
}

int VooDooGraphics::_Clear(COLORREF col)
{
	HBRUSH hBrush = CreateSolidBrush(col); // �ܻ� �귯�� ����
	RECT rect = { static_cast<long>(m_outputMerge.m_viewport.X), static_cast<long>(m_outputMerge.m_viewport.Y),
				  static_cast<long>(m_outputMerge.m_viewport.X + m_outputMerge.presentParameters.BackBufferWidth),
				  static_cast<long>(m_outputMerge.m_viewport.Y + m_outputMerge.presentParameters.BackBufferHeight) }; // Ŭ���� ���� ����

	FillRect(m_outputMerge.hDC, &rect, hBrush); // DC�� �귯�÷� ä���
	DeleteObject(hBrush); // �귯�� ����

	return 0;
}

int VooDooGraphics::_Present()
{
	HDC hDC = GetDC(m_outputMerge.hWnd); // ������ DC ��������

	BitBlt(hDC, 0, 0, m_outputMerge.presentParameters.BackBufferWidth, m_outputMerge.presentParameters.BackBufferHeight, m_outputMerge.hDC, 0, 0, SRCCOPY); // �� ���۸� ȭ�鿡 ����
	ReleaseDC(m_outputMerge.hWnd, hDC); // DC ����

	return 0;
}

int VooDooGraphics::_VertexStreaming (VDVECTOR oPos[3], VDCOLOR oColor[3])
{
	if (m_inputSystem.pVertexBuffer[0] == nullptr) { return -1; } // ���� ���۰� ������ ����
	if (m_inputSystem.Stride == 0) { return -2; } // ��Ʈ���̵尡 0�̸� ����

	VDVertexBuffer* pVB = GetClassObject<VDVertexBuffer>(m_inputSystem.pVertexBuffer[0]); // ���ؽ� ���� ��ü ��������
	if (pVB == nullptr) { return -3; } // ���� ���۰� ������ ����

	UINT vertexCount = pVB->GetVertexAmount(); // ���� ���� ��������

	m_streamDbg.currentPrimitiveIndex = m_streamDbg.currentVertexIndex / m_inputSystem.VertexAmount; // ���� ���� ���� �ε��� ���
	if (m_streamDbg.currentVertexIndex + m_inputSystem.VertexAmount > vertexCount) { return -4; } // ���� ���� �ε����� ���� ���� ������ ���� ������ �ʰ��ϸ� ����

	for (UINT i = 0; i < m_inputSystem.VertexAmount; ++i)
	{
		UINT v = m_streamDbg.currentVertexIndex + i; // ���� ���� �ε��� + i

		oPos[i] = pVB->_GetVertexPosition(v); // ���� ��ġ ��������
		oColor[i] = pVB->_GetDiffuse(v); // ���� ���� ��������
	}

	return 0;
}

int VooDooGraphics::_VertexPipeline(_In_ VDVECTOR iPos[3], _Out_ VDVECTOR oPos[3], _Inout_ VDCOLOR oColor[3])
{
	std::vector<VS_OUTPUT> vsOutputs(m_inputSystem.VertexAmount); // VS_OUTPUT ����ü ���� ����
	for (UINT i = 0; i < m_inputSystem.VertexAmount; ++i)
	{
		VDVECTOR& pos = iPos[i]; // �Է� ���� ��ġ
		VDCOLOR& color = oColor[i]; // �Է� ���� ����

		vsOutputs[i] = _TnL_VP(pos, color); // ��ȯ �� ���� ó��
	}

	_PrimitiveAssembly(vsOutputs, oPos, oColor); // ���� ���� ����

	return 0;
}

int VooDooGraphics::_GeometryPipeline(_In_ VDVECTOR iPos[3], _Out_ VDVECTOR oPos[3], _Inout_ VDCOLOR oColor[3])
{
	int result = 0;
	result = _TnL_GP(iPos, oPos, oColor); // ��ȯ �� ���� ó��

	return 0;
}

int VooDooGraphics::_PixelPipeline(_In_ VDVECTOR oPos[3], _In_ VDCOLOR oColor[3])
{
	VDVECTOR pos[3]; // ���� ��ġ �迭
	for (UINT i = 0; i < 3; ++i)
	{
		pos[i] = oPos[i]; // ��� ���� ��ġ�� �Է� ���� ��ġ�� ����
	}

	// �ȼ� ���������� // ������ ��� ����
	_PixelPipeline1();
	_PixelPipeline2();
	if (m_pipelineState.renderState[VDRS_FILLMODE] == VDFILL_WIREFRAME) // ���� ���̾������� ��� ���
	{
		_Line(pos[0], pos[1], oColor[0], oColor[1]); // �� �׸���
		_Line(pos[1], pos[2], oColor[1], oColor[2]); // �� �׸���
		_Line(pos[2], pos[0], oColor[2], oColor[0]); // ������ �� �׸���
		return 0;
	}
	else
	{
		_Face(pos, oColor); // �ﰢ�� �׸���
		return 0;
	}

	return 0;
}

int VooDooGraphics::_PrimitiveAssembly(_In_ std::vector<VS_OUTPUT>& vsouts, _Out_ VDVECTOR oPos[3], _Out_ VDCOLOR oColor[3])
{
	for (UINT i = 0; i < m_inputSystem.VertexAmount; ++i)
	{
		oPos[i] = vsouts[i].Pos; // ��ȯ�� ���� ��ġ
		oColor[i] = vsouts[i].Color; // ��ȯ�� ���� ����
	}
	return 0;
}

int VooDooGraphics::_PixelPipeline1()
{
	return 0;
}

int VooDooGraphics::_PixelPipeline2()
{
	return 0;
}

bool VooDooGraphics::_FaceCulling2(VDVECTOR v0, VDVECTOR v1, VDVECTOR v2)
{
	bool isCull = false; // �ø� ���� �ʱ�ȭ
	VDVECTOR u;
	VDVECTOR v;
	VDVECTOR normal;

	switch (m_pipelineState.renderState[VDRS_CULLMODE])
	{
		case VDCULL_NONE: // �ø� ����
			break;

		case VDCULL_CW: // �ð� ���� �ø�
			u = v1 - v0; // v0���� v1������ ����
			v = v2 - v0; // v0���� v2������ ����
			normal = u ^ v; // ������ �̿��� ���� ���� ���
			if (normal.z < 0.0f) { isCull = true; } // ���� ������ z���� �����̸� �ø�
			break;

		case VDCULL_CCW: // �ݽð� ���� �ø�
			u = v1 - v0; // v0���� v1������ ����
			v = v2 - v0; // v0���� v2������ ����
			normal = u ^ v; // ������ �̿��� ���� ���� ���
			if (normal.z > 0.0f) { isCull = true; } // ���� ������ z���� ����̸� �ø�
			break;
	}

	return isCull;
}

bool VooDooGraphics::_FaceCulling3(VDVECTOR v0, VDVECTOR v1, VDVECTOR v2)
{
	return _FaceCulling2(v0, v1, v2); // _FaceCulling2 ��� // ����� 2D �ø��� �����Ǿ� �����Ƿ� 3D �ø��� �����ϰ� ó��
}

bool VooDooGraphics::_FrustumCulling(VDVECTOR v[3])
{
	bool isCull = false; // �ø� ���� �ʱ�ȭ

	for (UINT i = 0; i < m_inputSystem.VertexAmount; ++i)
	{
		float x = v[i].x / v[i].w; // x ��ǥ ���� ��ǥ ������
		float y = v[i].y / v[i].w; // y ��ǥ ���� ��ǥ ������
		float z = v[i].z / v[i].w; // z ��ǥ ���� ��ǥ ������

		if (x < -1.0f || x > 1.0f || y < -1.0f || y > 1.0f || z < 0.0f || z > 1.0f)
		{
			isCull = true;
			break;
		}
	}

	return isCull;
}

int VooDooGraphics::_HomogeneousDivide(VDVECTOR iPos[3], VDVECTOR oPos[3])
{
	for (UINT i = 0; i < m_inputSystem.VertexAmount; ++i)
	{
		if (iPos[i].w == 0.0f) { return -1; } // w�� 0�̸� ����

		oPos[i].x = iPos[i].x / iPos[i].w; // x ��ǥ ���� ��ǥ ������
		oPos[i].y = iPos[i].y / iPos[i].w; // y ��ǥ ���� ��ǥ ������
		oPos[i].z = iPos[i].z / iPos[i].w; // z ��ǥ ���� ��ǥ ������
		oPos[i].w = 1.0f; // w�� �׻� 1��
	}

	return 0;
}

int VooDooGraphics::_Viewport(VDVECTOR iPos[3], VDVECTOR oPos[3])
{
	float w = static_cast<float>(m_outputMerge.m_viewport.Width);
	float h = static_cast<float>(m_outputMerge.m_viewport.Height);

	int x = static_cast<int>(m_outputMerge.m_viewport.X);
	int y = static_cast<int>(m_outputMerge.m_viewport.Y);

	for (UINT i = 0; i < m_inputSystem.VertexAmount; ++i)
	{
		oPos[i].x = static_cast<float>(x + (iPos[i].x + 1.0f) * 0.5f * w); // x ��ǥ ����Ʈ ��ȯ
		oPos[i].y = static_cast<float>(y + (1.0f - (iPos[i].y + 1.0f) * 0.5f) * h); // y ��ǥ ����Ʈ ��ȯ
	}

	return 0;
}

VS_OUTPUT VooDooGraphics::_TnL_VP(VDVECTOR iPos, VDCOLOR iColor)
{
	VS_OUTPUT output;

	VDVECTOR pos = iPos; // �Է� ���� ��ġ
	VDVECTOR transformedPos =
		pos.Transform(m_transform.transform[VDTS_WORLD]) // ���� ��ȯ ����� �̿��� ���� ��ġ ��ȯ
		.Transform(m_transform.transform[VDTS_VIEW]) // �� ��ȯ ����� �̿��� ���� ��ġ ��ȯ
		.Transform(m_transform.transform[VDTS_PROJECTION]); // �������� ��ȯ ����� �̿��� ���� ��ġ ��ȯ

	output.Pos = transformedPos; // ��ȯ�� ���� ��ġ ����
	output.Color = iColor; // �Է� ���� ���� ����

	return output;
}

int VooDooGraphics::_TnL_GP(_In_ VDVECTOR iPos4[3], _Out_ VDVECTOR oPos[3], _Inout_ VDCOLOR oColor[3])
{
	VDVECTOR vDevSpacePos[3]; // ����̽� ���� ��ǥ�� ���� �迭
	VDVECTOR vScreenPos[3]; // ȭ�� ��ǥ�� ���� �迭

	if (m_pipelineState.renderState[VDRS_CLIPPING])
	{
		if (_FrustumCulling(iPos4)) // �������� �ø�
		{
			return -1;
		}
	}

	_HomogeneousDivide(iPos4, vDevSpacePos); // ���� ��ǥ ������
	_Viewport(vDevSpacePos, vScreenPos); // ����Ʈ ��ȯ

	for (UINT i = 0; i < m_inputSystem.VertexAmount; ++i)
	{
		oPos[i].x = vScreenPos[i].x;
		oPos[i].y = vScreenPos[i].y;
		oPos[i].z = vDevSpacePos[i].z; // z ��ǥ�� ���� ��ǥ ������ ���� ��
		oColor[i] = oColor[i]; // ������ �״�� ����
	}

	return 0;
}

int VooDooGraphics::_TnL_PP(_In_ VDVECTOR oPos[3], _In_ VDCOLOR oColor[3])
{
	return 0;
}

void VooDooGraphics::_initRenderState()
{
	::ZeroMemory(&m_pipelineState.renderState, sizeof(m_pipelineState.renderState)); // ���� ���� �ʱ�ȭ

	m_pipelineState.renderState[VDRS_FILLMODE] = VDFILL_SOLID; // �⺻ ä��� ��� ����
	m_pipelineState.renderState[VDRS_CULLMODE] = VDCULL_CCW; // �⺻ �ø� ��� ����
}

int VooDooGraphics::_SetViewport(VDVIEWPORT* pViewport)
{
	if (pViewport == nullptr) { return -1; } // ����Ʈ�� ������ ����

	m_outputMerge.m_viewport = *pViewport; // ����Ʈ ����

	if (m_outputMerge.m_hRGN) DeleteObject(m_outputMerge.m_hRGN); // ���� ����Ʈ ���� ����

	m_outputMerge.m_hRGN = CreateRectRgn(static_cast<int>(m_outputMerge.m_viewport.X),
		static_cast<int>(m_outputMerge.m_viewport.Y),
		static_cast<int>(m_outputMerge.m_viewport.X + m_outputMerge.m_viewport.Width),
		static_cast<int>(m_outputMerge.m_viewport.Y + m_outputMerge.m_viewport.Height)); // �� ����Ʈ ���� ����
	if (m_outputMerge.m_hRGN == nullptr) { return -2; } // ����Ʈ ���� ���� ���� �� ����

	SelectClipRgn(m_outputMerge.hDC, m_outputMerge.m_hRGN); // DC�� ����Ʈ ���� ����

	return 0;
}

int VooDooGraphics::_GetViewport(VDVIEWPORT* pViewport)
{
	if (pViewport == nullptr) { return -1; } // ����Ʈ�� ������ ����

	*pViewport = m_outputMerge.m_viewport; // ���� ����Ʈ ������ ��ȯ

	return 0;
}

int VooDooGraphics::_Line(VDVECTOR& v0, VDVECTOR& v1, VDCOLOR& color)
{
	HPEN hPen = CreatePen(PS_SOLID, 1, color);
	HPEN hOldPen = (HPEN)SelectObject(m_outputMerge.hDC, hPen);
	MoveToEx(m_outputMerge.hDC, static_cast<int>(v0.x), static_cast<int>(v0.y), nullptr);
	LineTo(m_outputMerge.hDC, static_cast<int>(v1.x), static_cast<int>(v1.y));
	SelectObject(m_outputMerge.hDC, hOldPen);
	DeleteObject(hPen);

	return 0;
}

int VooDooGraphics::_Line(VDVECTOR& v0, VDVECTOR& v1, VDCOLOR& color1, VDCOLOR& color2)
{
	int startX = static_cast<int>(v0.x);
	int startY = static_cast<int>(v0.y);
	int endX = static_cast<int>(v1.x);
	int endY = static_cast<int>(v1.y);
	int xLength = endX - startX;
	int yLength = endY - startY;
	int absX = abs(xLength);
	int absY = abs(yLength);

	if (absX && absY)
	{
		if (absX > absY)
		{
			for(int x = startX; x != endX; x += (xLength > 0 ? 1 : -1))
			{
				int y = startY + (x - startX) * yLength / xLength;
				VDCOLOR color = VDCOLOR::Lerp(color1, color2, static_cast<float>(x - startX) / absX);
				SetPixel(m_outputMerge.hDC, x, y, static_cast<COLORREF>(color));
			}
		}
		else
		{
			for(int y = startY; y != endY; y += (yLength > 0 ? 1 : -1))
			{
				int x = startX + (y - startY) * xLength / yLength;
				VDCOLOR color = VDCOLOR::Lerp(color1, color2, static_cast<float>(y - startY) / absY);
				SetPixel(m_outputMerge.hDC, x, y, static_cast<COLORREF>(color));
			}
		}
	}

	return 0;
}

int VooDooGraphics::_Face(VDVECTOR pos[3], VDCOLOR color[3])
{
	int minX = static_cast<int>(min(min(pos[0].x, pos[1].x), pos[2].x));
	int minY = static_cast<int>(min(min(pos[0].y, pos[1].y), pos[2].y));
	int maxX = static_cast<int>(max(max(pos[0].x, pos[1].x), pos[2].x));
	int maxY = static_cast<int>(max(max(pos[0].y, pos[1].y), pos[2].y));

	VDVECTOR u = pos[1] - pos[0];
	VDVECTOR v = pos[2] - pos[0];

	VDVECTOR cross = u ^ v; // ������ �̿��� ���� ���� ���
	if (cross.z == 0.0f) { return -1; } // ������ 0�̸� �ﰢ���� �ƴϹǷ� ����
	float invArea = 1.0f / cross.z; // ������ ����

	for (int y = minY; y <= maxY; ++y)
	{
		for (int x = minX; x <= maxX; ++x)
		{
			VDVECTOR p(x, y, 0.0f); // ���� �ȼ� ��ġ

			// �ٸ���Ʈ�� ��ǥ ���
			float alpha = ((pos[1].y - pos[2].y) * (p.x - pos[2].x) + (pos[2].x - pos[1].x) * (p.y - pos[2].y)) * invArea;
			float beta = ((pos[2].y - pos[0].y) * (p.x - pos[2].x) + (pos[0].x - pos[2].x) * (p.y - pos[2].y)) * invArea;
			float gamma = 1.0f - alpha - beta; // �� ��° ��ǥ ���
			if (alpha >= 0.0f && beta >= 0.0f && gamma >= 0.0f) // �ٸ���Ʈ�� ��ǥ�� ��� ����̸� �ȼ� ����
			{
				VDCOLOR colorPixel = color[0] * alpha + color[1] * beta + color[2] * gamma; // ���� ����
				SetPixel(m_outputMerge.hDC, x, y, colorPixel); // �ȼ� ���� ����
			}
		}
	}

	return 0;
}

void VooDooGraphics::_HLine(HDC hdc, int x1, int x2, int y, VDCOLOR color)
{
	VDCOLOR colorPixel = color; // ���� ����
	float a = 0;
	float length = static_cast<float>(abs(x2 - x1) + 1);

	if (length == 0) { SetPixel(hdc, x1, y, colorPixel); return; } // ���̰� 0�̸� �ȼ� �ϳ��� ���
	if (x1 > x2) { std::swap(x1, x2); }

	for (int i = 0; i < length; i++)
	{
		a = static_cast<float>(i) / length; // ���� ��� ���
		VDCOLOR colorPixel = VDCOLOR::Lerp(color, VDCOLOR(0, 0, 0, 0), a); // ���� ����
		SetPixel(hdc, x1 + i, y, static_cast<COLORREF>(colorPixel)); // �ȼ� ���� ����
	}
}

void VooDooGraphics::_DrawGrid()
{
	if (!m_pipelineState.renderState[VDRS_GRIDMODE]) return;

	constexpr int GRID_AMOUNT = 10; // �׸��� ũ��
	constexpr int GRID_SCALE = 1; // �׸��� ������
	constexpr int LINE_AMOUNT = GRID_AMOUNT / GRID_SCALE; // �׸��� ���� ����

	float sx = -GRID_AMOUNT / 2.0f; // ���� x ��ǥ
	float sz = GRID_AMOUNT / 2.0f; // ���� z ��ǥ

	VDCOLOR gridColorX(1, 1, 0, 1); // X �׸��� ����
	VDCOLOR gridColorZ(0, 1, 1, 1); // Z �׸��� ����

	VDVIEWPORT viewport = m_outputMerge.m_viewport; // ����Ʈ ����

	for (int z = 0; z <= LINE_AMOUNT; ++z)
	{
		VDVECTOR v0(sx, 0.0f, sz - z * GRID_SCALE);
		VDVECTOR v1(sx + GRID_AMOUNT, 0.0f, sz - z * GRID_SCALE);

		v0.VectorProjection(&viewport, &m_transform.transform[VDTS_PROJECTION], &m_transform.transform[VDTS_VIEW]);
		v1.VectorProjection(&viewport, &m_transform.transform[VDTS_PROJECTION], &m_transform.transform[VDTS_VIEW]);

		_Line(v0, v1, gridColorX);
	}
	for (int x = 0; x <= LINE_AMOUNT; ++x)
	{
		VDVECTOR v0(sx + x * GRID_SCALE, 0.0f, sz);
		VDVECTOR v1(sx + x * GRID_SCALE, 0.0f, sz - GRID_AMOUNT);

		v0.VectorProjection(&viewport, &m_transform.transform[VDTS_PROJECTION], &m_transform.transform[VDTS_VIEW]);
		v1.VectorProjection(&viewport, &m_transform.transform[VDTS_PROJECTION], &m_transform.transform[VDTS_VIEW]);

		_Line(v0, v1, gridColorZ);
	}
}

void VooDooGraphics::_DrawAxis()
{
	if (!m_pipelineState.renderState[VDRS_AXISMODE]) return;

	VDVECTOR v0(0.0f, 0.0f, 0.0f); // ����
	VDVECTOR v1(1.0f, 0.0f, 0.0f); // X ��
	VDVECTOR v2(0.0f, 1.0f, 0.0f); // Y ��
	VDVECTOR v3(0.0f, 0.0f, 1.0f); // Z ��

	VDVIEWPORT viewport = m_outputMerge.m_viewport; // ����Ʈ ����

	v0.VectorProjection(&viewport, &m_transform.transform[VDTS_PROJECTION], &m_transform.transform[VDTS_VIEW]); // ���� ����
	v1.VectorProjection(&viewport, &m_transform.transform[VDTS_PROJECTION], &m_transform.transform[VDTS_VIEW]); // X �� ����
	v2.VectorProjection(&viewport, &m_transform.transform[VDTS_PROJECTION], &m_transform.transform[VDTS_VIEW]); // Y �� ����
	v3.VectorProjection(&viewport, &m_transform.transform[VDTS_PROJECTION], &m_transform.transform[VDTS_VIEW]); // Z �� ����

	VDCOLOR colorX(1, 0, 0, 1); // X �� ����
	VDCOLOR colorY(0, 1, 0, 1); // Y �� ����
	VDCOLOR colorZ(0, 0, 1, 1); // Z �� ����

	_Line(v0, v1, colorX); // X �� �׸���
	_Line(v0, v2, colorY); // Y �� �׸���
	_Line(v0, v3, colorZ); // Z �� �׸���
}

template <typename T>
T* GetClassObject(IVooDooVertexBuffer* pVertexBuffer)
{
	return dynamic_cast<T*>(pVertexBuffer);
}