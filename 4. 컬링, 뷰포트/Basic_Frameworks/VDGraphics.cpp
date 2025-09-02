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
	if (m_inputSystem.pVertexBuffer[0] == nullptr) { return -1; } // 정점 버퍼가 없으면 에러
	if (m_inputSystem.Stride == 0) { return -2; } // 스트라이드가 0이면 에러

	VDVECTOR vPos[3]; // 정점 좌표
	VDVECTOR oPos3[3]; // 정점 좌표 // 위에 vPos와 다른 건가? // 아마도 출력용?
	VDVECTOR oPos4[3]; // 4차원 정점 좌표?
	VDCOLOR oColor[3]; // 정점 색상

	m_streamDbg.currentVertexIndex = m_inputSystem.StartVertex; // 현재 정점 인덱스 초기화
	m_streamDbg.currentPrimitiveIndex = -1; // 현재 도형 형태 인덱스 초기화
	m_streamDbg.currentPrimitiveDrawCount = 0; // 현재 그리고 있는 도형 개수 초기화

	int result = 0;

	while (true)
	{
		result = _VertexStreaming(vPos, oColor); // 정점 스트리밍
		if (result < 0) { return result; } // 에러 발생 시 반환

		_VertexPipeline(vPos, oPos4, oColor); // 정점 파이프라인 처리

		result = _GeometryPipeline(oPos4, oPos3, oColor); // 기하학 파이프라인 처리
		if (result < 0) { return result; } // 에러 발생 시 반환

		_PixelPipeline(oPos3, oColor); // 픽셀 파이프라인 처리

		m_streamDbg.currentVertexIndex += m_inputSystem.VertexAmount; // 현재 정점 인덱스 업데이트
		m_streamDbg.currentPrimitiveIndex = m_streamDbg.currentVertexIndex / m_inputSystem.VertexAmount; // 현재 도형 형태 인덱스 업데이트

		if (++m_streamDbg.currentPrimitiveDrawCount >= m_inputSystem.PrimitiveCount)
		{
			break;
		}
	}

	return 0;
}

int VooDooGraphics::Release()
{
	_RenderTargetRelease(); // 렌더 타겟 해제

	_TempBufferRelease(); // 임시 버퍼 해제 // 아직 기능 없음
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
	if (pVB == nullptr) { return -1; } // 버텍스 버퍼가 없으면 에러

	m_inputSystem.pVertexBuffer[0] = pVB; // 버텍스 버퍼 설정
	m_inputSystem.Stride = Stride; // 스트라이드 설정

	return 0;
}

int VooDooGraphics::_SetFVF(DWORD fvf)
{
	m_inputSystem.FVF = fvf; // FVF 설정

	return 0;
}

void VooDooGraphics::_TempBufferRelease()
{
}

int VooDooGraphics::_BeginScene()
{
	SetBkMode(m_outputMerge.hDC, TRANSPARENT); // 백 모드 설정 // 투명

	m_hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255)); // 흰색 펜 생성 // CreatePen 사용
	m_hOldPen = (HPEN)SelectObject(m_outputMerge.hDC, m_hPen); // DC에 펜 선택

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
	HBRUSH hBrush = CreateSolidBrush(col); // 단색 브러시 생성
	RECT rect = { static_cast<long>(m_outputMerge.m_viewport.X), static_cast<long>(m_outputMerge.m_viewport.Y),
				  static_cast<long>(m_outputMerge.m_viewport.X + m_outputMerge.presentParameters.BackBufferWidth),
				  static_cast<long>(m_outputMerge.m_viewport.Y + m_outputMerge.presentParameters.BackBufferHeight) }; // 클리어 영역 설정

	FillRect(m_outputMerge.hDC, &rect, hBrush); // DC에 브러시로 채우기
	DeleteObject(hBrush); // 브러시 삭제

	return 0;
}

int VooDooGraphics::_Present()
{
	HDC hDC = GetDC(m_outputMerge.hWnd); // 윈도우 DC 가져오기

	BitBlt(hDC, 0, 0, m_outputMerge.presentParameters.BackBufferWidth, m_outputMerge.presentParameters.BackBufferHeight, m_outputMerge.hDC, 0, 0, SRCCOPY); // 백 버퍼를 화면에 복사
	ReleaseDC(m_outputMerge.hWnd, hDC); // DC 해제

	return 0;
}

int VooDooGraphics::_VertexStreaming (VDVECTOR oPos[3], VDCOLOR oColor[3])
{
	if (m_inputSystem.pVertexBuffer[0] == nullptr) { return -1; } // 정점 버퍼가 없으면 에러
	if (m_inputSystem.Stride == 0) { return -2; } // 스트라이드가 0이면 에러

	VDVertexBuffer* pVB = GetClassObject<VDVertexBuffer>(m_inputSystem.pVertexBuffer[0]); // 버텍스 버퍼 객체 가져오기
	if (pVB == nullptr) { return -3; } // 정점 버퍼가 없으면 에러

	UINT vertexCount = pVB->GetVertexAmount(); // 정점 개수 가져오기

	m_streamDbg.currentPrimitiveIndex = m_streamDbg.currentVertexIndex / m_inputSystem.VertexAmount; // 현재 도형 형태 인덱스 계산
	if (m_streamDbg.currentVertexIndex + m_inputSystem.VertexAmount > vertexCount) { return -4; } // 현재 정점 인덱스와 도형 형태 개수가 정점 개수를 초과하면 에러

	for (UINT i = 0; i < m_inputSystem.VertexAmount; ++i)
	{
		UINT v = m_streamDbg.currentVertexIndex + i; // 현재 정점 인덱스 + i

		oPos[i] = pVB->_GetVertexPosition(v); // 정점 위치 가져오기
		oColor[i] = pVB->_GetDiffuse(v); // 정점 색상 가져오기
	}

	return 0;
}

int VooDooGraphics::_VertexPipeline(_In_ VDVECTOR iPos[3], _Out_ VDVECTOR oPos[3], _Inout_ VDCOLOR oColor[3])
{
	std::vector<VS_OUTPUT> vsOutputs(m_inputSystem.VertexAmount); // VS_OUTPUT 구조체 벡터 생성
	for (UINT i = 0; i < m_inputSystem.VertexAmount; ++i)
	{
		VDVECTOR& pos = iPos[i]; // 입력 정점 위치
		VDCOLOR& color = oColor[i]; // 입력 정점 색상

		vsOutputs[i] = _TnL_VP(pos, color); // 변환 및 조명 처리
	}

	_PrimitiveAssembly(vsOutputs, oPos, oColor); // 도형 형태 조립

	return 0;
}

int VooDooGraphics::_GeometryPipeline(_In_ VDVECTOR iPos[3], _Out_ VDVECTOR oPos[3], _Inout_ VDCOLOR oColor[3])
{
	int result = 0;
	result = _TnL_GP(iPos, oPos, oColor); // 변환 및 조명 처리

	return 0;
}

int VooDooGraphics::_PixelPipeline(_In_ VDVECTOR oPos[3], _In_ VDCOLOR oColor[3])
{
	VDVECTOR pos[3]; // 정점 위치 배열
	for (UINT i = 0; i < 3; ++i)
	{
		pos[i] = oPos[i]; // 출력 정점 위치를 입력 정점 위치로 설정
	}

	// 픽셀 파이프라인 // 지금은 기능 없음
	_PixelPipeline1();
	_PixelPipeline2();
	if (m_pipelineState.renderState[VDRS_FILLMODE] == VDFILL_WIREFRAME) // 만약 와이어프레임 모드 라면
	{
		_Line(pos[0], pos[1], oColor[0], oColor[1]); // 선 그리기
		_Line(pos[1], pos[2], oColor[1], oColor[2]); // 선 그리기
		_Line(pos[2], pos[0], oColor[2], oColor[0]); // 마지막 선 그리기
		return 0;
	}
	else
	{
		_Face(pos, oColor); // 삼각형 그리기
		return 0;
	}

	return 0;
}

int VooDooGraphics::_PrimitiveAssembly(_In_ std::vector<VS_OUTPUT>& vsouts, _Out_ VDVECTOR oPos[3], _Out_ VDCOLOR oColor[3])
{
	for (UINT i = 0; i < m_inputSystem.VertexAmount; ++i)
	{
		oPos[i] = vsouts[i].Pos; // 변환된 정점 위치
		oColor[i] = vsouts[i].Color; // 변환된 정점 색상
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
	bool isCull = false; // 컬링 여부 초기화
	VDVECTOR u;
	VDVECTOR v;
	VDVECTOR normal;

	switch (m_pipelineState.renderState[VDRS_CULLMODE])
	{
		case VDCULL_NONE: // 컬링 없음
			break;

		case VDCULL_CW: // 시계 방향 컬링
			u = v1 - v0; // v0에서 v1까지의 벡터
			v = v2 - v0; // v0에서 v2까지의 벡터
			normal = u ^ v; // 외적을 이용한 법선 벡터 계산
			if (normal.z < 0.0f) { isCull = true; } // 법선 벡터의 z값이 음수이면 컬링
			break;

		case VDCULL_CCW: // 반시계 방향 컬링
			u = v1 - v0; // v0에서 v1까지의 벡터
			v = v2 - v0; // v0에서 v2까지의 벡터
			normal = u ^ v; // 외적을 이용한 법선 벡터 계산
			if (normal.z > 0.0f) { isCull = true; } // 법선 벡터의 z값이 양수이면 컬링
			break;
	}

	return isCull;
}

bool VooDooGraphics::_FaceCulling3(VDVECTOR v0, VDVECTOR v1, VDVECTOR v2)
{
	return _FaceCulling2(v0, v1, v2); // _FaceCulling2 사용 // 현재는 2D 컬링만 구현되어 있으므로 3D 컬링은 동일하게 처리
}

bool VooDooGraphics::_FrustumCulling(VDVECTOR v[3])
{
	bool isCull = false; // 컬링 여부 초기화

	for (UINT i = 0; i < m_inputSystem.VertexAmount; ++i)
	{
		float x = v[i].x / v[i].w; // x 좌표 동차 좌표 나누기
		float y = v[i].y / v[i].w; // y 좌표 동차 좌표 나누기
		float z = v[i].z / v[i].w; // z 좌표 동차 좌표 나누기

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
		if (iPos[i].w == 0.0f) { return -1; } // w가 0이면 에러

		oPos[i].x = iPos[i].x / iPos[i].w; // x 좌표 동차 좌표 나누기
		oPos[i].y = iPos[i].y / iPos[i].w; // y 좌표 동차 좌표 나누기
		oPos[i].z = iPos[i].z / iPos[i].w; // z 좌표 동차 좌표 나누기
		oPos[i].w = 1.0f; // w는 항상 1로
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
		oPos[i].x = static_cast<float>(x + (iPos[i].x + 1.0f) * 0.5f * w); // x 좌표 뷰포트 변환
		oPos[i].y = static_cast<float>(y + (1.0f - (iPos[i].y + 1.0f) * 0.5f) * h); // y 좌표 뷰포트 변환
	}

	return 0;
}

VS_OUTPUT VooDooGraphics::_TnL_VP(VDVECTOR iPos, VDCOLOR iColor)
{
	VS_OUTPUT output;

	VDVECTOR pos = iPos; // 입력 정점 위치
	VDVECTOR transformedPos =
		pos.Transform(m_transform.transform[VDTS_WORLD]) // 월드 변환 행렬을 이용한 정점 위치 변환
		.Transform(m_transform.transform[VDTS_VIEW]) // 뷰 변환 행렬을 이용한 정점 위치 변환
		.Transform(m_transform.transform[VDTS_PROJECTION]); // 프로젝션 변환 행렬을 이용한 정점 위치 변환

	output.Pos = transformedPos; // 변환된 정점 위치 설정
	output.Color = iColor; // 입력 정점 색상 설정

	return output;
}

int VooDooGraphics::_TnL_GP(_In_ VDVECTOR iPos4[3], _Out_ VDVECTOR oPos[3], _Inout_ VDCOLOR oColor[3])
{
	VDVECTOR vDevSpacePos[3]; // 디바이스 공간 좌표용 정점 배열
	VDVECTOR vScreenPos[3]; // 화면 좌표용 정점 배열

	if (m_pipelineState.renderState[VDRS_CLIPPING])
	{
		if (_FrustumCulling(iPos4)) // 프러스텀 컬링
		{
			return -1;
		}
	}

	_HomogeneousDivide(iPos4, vDevSpacePos); // 동차 좌표 나누기
	_Viewport(vDevSpacePos, vScreenPos); // 뷰포트 변환

	for (UINT i = 0; i < m_inputSystem.VertexAmount; ++i)
	{
		oPos[i].x = vScreenPos[i].x;
		oPos[i].y = vScreenPos[i].y;
		oPos[i].z = vDevSpacePos[i].z; // z 좌표는 동차 좌표 나누기 후의 값
		oColor[i] = oColor[i]; // 색상은 그대로 유지
	}

	return 0;
}

int VooDooGraphics::_TnL_PP(_In_ VDVECTOR oPos[3], _In_ VDCOLOR oColor[3])
{
	return 0;
}

void VooDooGraphics::_initRenderState()
{
	::ZeroMemory(&m_pipelineState.renderState, sizeof(m_pipelineState.renderState)); // 렌더 상태 초기화

	m_pipelineState.renderState[VDRS_FILLMODE] = VDFILL_SOLID; // 기본 채우기 모드 설정
	m_pipelineState.renderState[VDRS_CULLMODE] = VDCULL_CCW; // 기본 컬링 모드 설정
}

int VooDooGraphics::_SetViewport(VDVIEWPORT* pViewport)
{
	if (pViewport == nullptr) { return -1; } // 뷰포트가 없으면 에러

	m_outputMerge.m_viewport = *pViewport; // 뷰포트 설정

	if (m_outputMerge.m_hRGN) DeleteObject(m_outputMerge.m_hRGN); // 이전 뷰포트 영역 삭제

	m_outputMerge.m_hRGN = CreateRectRgn(static_cast<int>(m_outputMerge.m_viewport.X),
		static_cast<int>(m_outputMerge.m_viewport.Y),
		static_cast<int>(m_outputMerge.m_viewport.X + m_outputMerge.m_viewport.Width),
		static_cast<int>(m_outputMerge.m_viewport.Y + m_outputMerge.m_viewport.Height)); // 새 뷰포트 영역 생성
	if (m_outputMerge.m_hRGN == nullptr) { return -2; } // 뷰포트 영역 생성 실패 시 에러

	SelectClipRgn(m_outputMerge.hDC, m_outputMerge.m_hRGN); // DC에 뷰포트 영역 설정

	return 0;
}

int VooDooGraphics::_GetViewport(VDVIEWPORT* pViewport)
{
	if (pViewport == nullptr) { return -1; } // 뷰포트가 없으면 에러

	*pViewport = m_outputMerge.m_viewport; // 현재 뷰포트 정보를 반환

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

	VDVECTOR cross = u ^ v; // 외적을 이용한 법선 벡터 계산
	if (cross.z == 0.0f) { return -1; } // 면적이 0이면 삼각형이 아니므로 에러
	float invArea = 1.0f / cross.z; // 면적의 역수

	for (int y = minY; y <= maxY; ++y)
	{
		for (int x = minX; x <= maxX; ++x)
		{
			VDVECTOR p(x, y, 0.0f); // 현재 픽셀 위치

			// 바리센트릭 좌표 계산
			float alpha = ((pos[1].y - pos[2].y) * (p.x - pos[2].x) + (pos[2].x - pos[1].x) * (p.y - pos[2].y)) * invArea;
			float beta = ((pos[2].y - pos[0].y) * (p.x - pos[2].x) + (pos[0].x - pos[2].x) * (p.y - pos[2].y)) * invArea;
			float gamma = 1.0f - alpha - beta; // 세 번째 좌표 계산
			if (alpha >= 0.0f && beta >= 0.0f && gamma >= 0.0f) // 바리센트릭 좌표가 모두 양수이면 픽셀 내부
			{
				VDCOLOR colorPixel = color[0] * alpha + color[1] * beta + color[2] * gamma; // 색상 보간
				SetPixel(m_outputMerge.hDC, x, y, colorPixel); // 픽셀 색상 설정
			}
		}
	}

	return 0;
}

void VooDooGraphics::_HLine(HDC hdc, int x1, int x2, int y, VDCOLOR color)
{
	VDCOLOR colorPixel = color; // 색상 설정
	float a = 0;
	float length = static_cast<float>(abs(x2 - x1) + 1);

	if (length == 0) { SetPixel(hdc, x1, y, colorPixel); return; } // 길이가 0이면 픽셀 하나만 찍기
	if (x1 > x2) { std::swap(x1, x2); }

	for (int i = 0; i < length; i++)
	{
		a = static_cast<float>(i) / length; // 보간 계수 계산
		VDCOLOR colorPixel = VDCOLOR::Lerp(color, VDCOLOR(0, 0, 0, 0), a); // 색상 보간
		SetPixel(hdc, x1 + i, y, static_cast<COLORREF>(colorPixel)); // 픽셀 색상 설정
	}
}

void VooDooGraphics::_DrawGrid()
{
	if (!m_pipelineState.renderState[VDRS_GRIDMODE]) return;

	constexpr int GRID_AMOUNT = 10; // 그리드 크기
	constexpr int GRID_SCALE = 1; // 그리드 스케일
	constexpr int LINE_AMOUNT = GRID_AMOUNT / GRID_SCALE; // 그리드 라인 개수

	float sx = -GRID_AMOUNT / 2.0f; // 시작 x 좌표
	float sz = GRID_AMOUNT / 2.0f; // 시작 z 좌표

	VDCOLOR gridColorX(1, 1, 0, 1); // X 그리드 색상
	VDCOLOR gridColorZ(0, 1, 1, 1); // Z 그리드 색상

	VDVIEWPORT viewport = m_outputMerge.m_viewport; // 뷰포트 설정

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

	VDVECTOR v0(0.0f, 0.0f, 0.0f); // 원점
	VDVECTOR v1(1.0f, 0.0f, 0.0f); // X 축
	VDVECTOR v2(0.0f, 1.0f, 0.0f); // Y 축
	VDVECTOR v3(0.0f, 0.0f, 1.0f); // Z 축

	VDVIEWPORT viewport = m_outputMerge.m_viewport; // 뷰포트 설정

	v0.VectorProjection(&viewport, &m_transform.transform[VDTS_PROJECTION], &m_transform.transform[VDTS_VIEW]); // 원점 투영
	v1.VectorProjection(&viewport, &m_transform.transform[VDTS_PROJECTION], &m_transform.transform[VDTS_VIEW]); // X 축 투영
	v2.VectorProjection(&viewport, &m_transform.transform[VDTS_PROJECTION], &m_transform.transform[VDTS_VIEW]); // Y 축 투영
	v3.VectorProjection(&viewport, &m_transform.transform[VDTS_PROJECTION], &m_transform.transform[VDTS_VIEW]); // Z 축 투영

	VDCOLOR colorX(1, 0, 0, 1); // X 축 색상
	VDCOLOR colorY(0, 1, 0, 1); // Y 축 색상
	VDCOLOR colorZ(0, 0, 1, 1); // Z 축 색상

	_Line(v0, v1, colorX); // X 축 그리기
	_Line(v0, v2, colorY); // Y 축 그리기
	_Line(v0, v3, colorZ); // Z 축 그리기
}

template <typename T>
T* GetClassObject(IVooDooVertexBuffer* pVertexBuffer)
{
	return dynamic_cast<T*>(pVertexBuffer);
}