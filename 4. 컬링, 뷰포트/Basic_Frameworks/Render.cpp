#include <cmath>
#include "Render.h"
#include "Device.h"
#include "VDX9types.h"

const WCHAR* g_WindowName = L"VooDoo Device Test";

IVooDooVertexBuffer* g_pVertexBuffer = nullptr; // 전역 정점 버퍼 포인터

struct COLORVERTEX // 정점 구조체 정의
{
	float x, y, z;
	DWORD color;
};

const DWORD FVF_COLORVERTEX = VD3D | VDDIFFUSE; // FVF 플래그 정의

// 전역 변수 정의
BOOL g_bWireframe = FALSE;
BOOL g_bShowGrid = TRUE;
BOOL g_bShowAxis = TRUE;
BOOL g_bClipping = TRUE;
BOOL g_bViewport = TRUE;

VDCOLOR g_ClearColor = VDCOLOR(0.2f, 0.2f, 0.2f, 1.0f);

VDMATRIX g_mTM;
VDMATRIX g_mView;
VDMATRIX g_mProj;

float speed = 5.0f; // 캐릭터 이동 속도
float characterLocationMatrix[4][4] =
{
	{ 1.0f, 0.0f, 0.0f, 0.0f },
	{ 0.0f, 1.0f, 0.0f, 0.0f },
	{ 0.0f, 0.0f, 1.0f, 0.0f },
	{ 0.0f, 0.0f, 0.0f, 1.0f }
}; // 캐릭터 위치

VDVIEWPORT g_viewport = { 0, 0, 1600, 600, 0.0f, 1.0f }; // 기본 뷰포트 설정

constexpr int VIEWPORT_COUNT = 2; // 뷰포트 개수
VDVIEWPORT g_viewports[VIEWPORT_COUNT] =
{
	{ 0, 0, 800, 600, 0.0f, 1.0f },
	{ 800, 0, 800, 600, 0.0f, 1.0f }
};
bool g_viewportProject[VIEWPORT_COUNT] = { false, true }; // 뷰포트 프로젝션 여부

// 직교 투영 행렬 설정
float orthographicCamMatrix[4][4] =
{
	{ 1.0f, 0.0f, 0.0f, 0.0f },
	{ 0.0f, 1.0f, 0.0f, 0.0f },
	{ 0.0f, 0.0f, 0.0f, 0.0f },
	{ 0.0f, 0.0f, 1.0f, 10.0f }
};
VDMATRIX orthographicCamProjection(false, orthographicCamMatrix);

// 원근 투영 행렬 설정
float fov = rad(45.0f);
float aspectRatio = static_cast<float>(g_displayMode.Width) / static_cast<float>(g_displayMode.Height);
float nearPlane = 0.1f;
float farPlane = 100.0f;

// 더미 행렬들
float dummytranslationMatrix0[4][4] =
{
	{ 1.0f, 0.0f, 0.0f, 0.0f },
	{ 0.0f, 1.0f, 0.0f, 0.0f },
	{ 0.0f, 0.0f, 1.0f, 0.0f },
	{ 3.0f, 0.0f, 0.0f, 1.0f }
};
float dummytranslationMatrix1[4][4] =
{
	{ 1.0f, 0.0f, 0.0f, 0.0f },
	{ 0.0f, 1.0f, 0.0f, 0.0f },
	{ 0.0f, 0.0f, 1.0f, 0.0f },
	{ -3.0f, 0.0f, 0.0f, 1.0f }
};
float dummytranslationMatrix2[4][4] =
{
	{ 1.0f, 0.0f, 0.0f, 0.0f },
	{ 0.0f, 1.0f, 0.0f, 0.0f },
	{ 0.0f, 0.0f, 1.0f, 0.0f },
	{ 0.0f, 0.0f, 3.0f, 1.0f }
};
float dummytranslationMatrix3[4][4] =
{
	{ 1.0f, 0.0f, 0.0f, 0.0f },
	{ 0.0f, 1.0f, 0.0f, 0.0f },
	{ 0.0f, 0.0f, 1.0f, 0.0f },
	{ 0.0f, 0.0f, -3.0f, 1.0f }
};


int DataLoading()
{
	ObjLoad();

	return 1;
}

void DataRelease()
{
	ObjRelease();
}

int ObjLoad()
{
	COLORVERTEX vertices[] =
	{
		{ -0.5f, 0.0f, 0.0f, VDCOLOR(1.0f, 0.0f, 0.0f, 1.0f) },
		{ 0.0f, 0.5f, 0.0f, VDCOLOR(0.0f, 1.0f, 0.0f, 1.0f) },
		{ 0.5f, 0.0f, 0.0f, VDCOLOR(0.0f, 0.0f, 1.0f, 1.0f) },

		{ -0.5f, 0.0f, 0.0f, VDCOLOR(1.0f, 0.0f, 0.0f, 1.0f) },
		{ 0.0f, -0.5f, 0.0f, VDCOLOR(0.0f, 1.0f, 0.0f, 1.0f) },
		{ 0.5f, 0.0f, 0.0f, VDCOLOR(0.0f, 0.0f, 1.0f, 1.0f) }
	};

	if (FAILED(g_pVooDooDevice->CreateVertexBuffer(sizeof(vertices), 0, FVF_COLORVERTEX, VDPOOL_DEFAULT, &g_pVertexBuffer, nullptr))) { return -1; }

	COLORVERTEX* pBuffer = nullptr;
	if (FAILED(g_pVertexBuffer->Lock(0, sizeof(vertices), (void**)&pBuffer, 0))) { return -2; }

	memcpy(pBuffer, vertices, sizeof(vertices));

	g_pVertexBuffer->Unlock();

	g_mProj.VDMatrixPerspectiveFovLH(fov, aspectRatio, nearPlane, farPlane); // 원근 투영 행렬 설정

	return 0;
}

void ObjRelease()
{
	if (g_pVertexBuffer)
	{
		g_pVertexBuffer->Release();
		g_pVertexBuffer = nullptr;
	}
}

void ObjUpdate(float deltaTime)
{
	static float angle = 0.0f;
	angle += rad(90.0f) * deltaTime;
	if (IsKeyDown(VK_LEFT)) characterLocationMatrix[3][0] -= speed * deltaTime; // 왼쪽 이동
	if (IsKeyDown(VK_RIGHT)) characterLocationMatrix[3][0] += speed * deltaTime; // 오른쪽 이동
	if (IsKeyDown(VK_UP)) characterLocationMatrix[3][2] += speed * deltaTime; // 앞쪽 이동
	if (IsKeyDown(VK_DOWN)) characterLocationMatrix[3][2] -= speed * deltaTime; // 뒤쪽 이동
	if (IsKeyDown(VK_SPACE)) characterLocationMatrix[3][1] += speed * deltaTime; // 위쪽 이동
	if (IsKeyDown(VK_SHIFT)) characterLocationMatrix[3][1] -= speed * deltaTime; // 아래쪽 이동

	float rotationXMatrix[4][4] = 
	{
		{ cosf(angle), -sinf(angle), 0.0f, 0.0f },
		{ sinf(angle), cosf(angle), 0.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	}; // x축 회전 행렬
	float rotationZMatrix[4][4] = 
	{
		{ cosf(angle), 0.0f, sinf(angle), 0.0f },
		{ 0.0f, 1.0f, 0.0f, 0.0f },
		{ -sinf(angle), 0.0f, cosf(angle), 0.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	}; // z축 회전 행렬

	VDMATRIX rotationX(true, rotationXMatrix);
	VDMATRIX rotationZ(true, rotationZMatrix);
	VDMATRIX translation(false, characterLocationMatrix);

	rotationZ *= translation;
	rotationZ.Transposition(); // 행렬 전치
	rotationX *= rotationZ; // Z축 회전 후 X축 회전

	g_mTM = rotationX; // 회전 행렬을 기존 변환 행렬에 곱함
}

void CameraUpdate(float deltaTime, bool isOrthographic)
{
	if (isOrthographic)
	{
		VDVECTOR cameraPosition(characterLocationMatrix[3][0], 10.0f, -10.0f + characterLocationMatrix[3][2]);
		VDVECTOR cameraTarget(characterLocationMatrix[3][0], 0.0f, characterLocationMatrix[3][2]);
		VDVECTOR cameraUp(0.0f, 1.0f, 0.0f);

		g_mView.VDMatrixLookAtLH(cameraPosition, cameraTarget, cameraUp); // 카메라 뷰 행렬 설정
		g_pVooDooDevice->SetTransform(VDTS_VIEW, &g_mView); // 뷰 행렬 설정
		g_pVooDooDevice->SetTransform(VDTS_PROJECTION, &orthographicCamProjection); // 정사영 투영 행렬 설정
	}
	else
	{
		VDVECTOR cameraPosition(5.0f, 5.0f, -10.0f);
		VDVECTOR cameraTarget(characterLocationMatrix[3][0], characterLocationMatrix[3][1], characterLocationMatrix[3][2]);
		VDVECTOR cameraUp(0.0f, 1.0f, 0.0f);

		g_mView.VDMatrixLookAtLH(cameraPosition, cameraTarget, cameraUp); // 카메라 뷰 행렬 설정
		g_pVooDooDevice->SetTransform(VDTS_VIEW, &g_mView); // 뷰 행렬 설정
		g_pVooDooDevice->SetTransform(VDTS_PROJECTION, &g_mProj); // 투영 행렬 설정
	}

}

void ObjDraw()
{
	if (g_pVertexBuffer)
	{
		g_pVooDooDevice->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(COLORVERTEX));
		g_pVooDooDevice->SetFVF(FVF_COLORVERTEX);
		g_pVooDooDevice->SetTransform(VDTS_WORLD, &g_mTM);
		g_pVooDooDevice->DrawPrimitive(VDPT_TRIANGLELIST, 0, 2);
	}
}

void DummyDraw()
{
	for (int i = 0; i < 4; ++i)
	{
		VDMATRIX dummytranslationMatrix(false, (i == 0) ? dummytranslationMatrix0 :
			(i == 1) ? dummytranslationMatrix1 :
			(i == 2) ? dummytranslationMatrix2 : dummytranslationMatrix3);

		dummytranslationMatrix.Transposition(); // 행렬 전치
		g_pVooDooDevice->SetTransform(VDTS_WORLD, &dummytranslationMatrix); // 월드 행렬 설정
		g_pVooDooDevice->DrawPrimitive(VDPT_TRIANGLELIST, 0, 2);
	}
}

void SystemUpdate()
{
	if (g_bWireframe) g_ClearColor = VDCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
	else g_ClearColor = VDCOLOR(0.2f, 0.2f, 0.2f, 1.0f);

	if (IsKeyUp(VK_F1)) g_bWireframe = !g_bWireframe;
	if (IsKeyUp(VK_F2)) g_bShowGrid = !g_bShowGrid;
	if (IsKeyUp(VK_F3)) g_bShowAxis = !g_bShowAxis;
	if (IsKeyUp(VK_F4)) g_bClipping = !g_bClipping;
	if (IsKeyUp(VK_F5)) g_bViewport = !g_bViewport;

	g_pVooDooDevice->SetRenderState(VDRS_FILLMODE, g_bWireframe ? VDFILL_WIREFRAME : VDFILL_SOLID);
	g_pVooDooDevice->SetRenderState(VDRS_GRIDMODE, g_bShowGrid ? TRUE : FALSE);
	g_pVooDooDevice->SetRenderState(VDRS_AXISMODE, g_bShowAxis ? TRUE : FALSE);
	g_pVooDooDevice->SetRenderState(VDRS_CLIPPING, g_bClipping ? TRUE : FALSE);
}

void SceneRender()
{
	SystemUpdate();

	float deltaTime = GetEngineTime();
	ObjUpdate(deltaTime);
	ShowFPS(10, 10);

	if (!g_bViewport)
	{
		CameraUpdate(deltaTime);

		g_pVooDooDevice->SetViewport(&g_viewport);

		g_pVooDooDevice->BeginScene();
		g_pVooDooDevice->Clear(g_ClearColor);

		g_pVooDooDevice->DrawGrid();
		g_pVooDooDevice->DrawAxis();

		DummyDraw();
		ObjDraw();

		g_pVooDooDevice->EndScene();
	}
	else
	{
		for (int i = 0; i < VIEWPORT_COUNT; ++i)
		{
			CameraUpdate(deltaTime, g_viewportProject[i]);

			g_pVooDooDevice->SetViewport(&g_viewports[i]);

			g_pVooDooDevice->BeginScene();
			g_pVooDooDevice->Clear(g_ClearColor);

			g_pVooDooDevice->DrawGrid();
			g_pVooDooDevice->DrawAxis();

			DummyDraw();
			ObjDraw();

			g_pVooDooDevice->EndScene();
		}
	}

	g_pVooDooDevice->Present();
}