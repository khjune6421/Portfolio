#include <cmath>
#include "Render.h"
#include "Device.h"
#include "VDX9types.h"

const WCHAR* g_WindowName = L"VooDoo Device Test";

IVooDooVertexBuffer* g_pVertexBuffer = nullptr;

struct COLORVERTEX
{
	float x, y, z;
	DWORD color;
};

const DWORD FVF_COLORVERTEX = VD3D | VDDIFFUSE;

BOOL g_bWireframe = FALSE;
BOOL g_bShowGrid = TRUE;
BOOL g_bShowAxis = TRUE;

VDCOLOR g_ClearColor = VDCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

VDMATRIX g_mTM;
VDMATRIX g_mView;
VDMATRIX g_mProj;

float speed = 5.0f; // ĳ���� �̵� �ӵ�
float characterLocationMatrix[4][4] =
{
	{ 1.0f, 0.0f, 0.0f, 0.0f },
	{ 0.0f, 1.0f, 0.0f, 0.0f },
	{ 0.0f, 0.0f, 1.0f, 0.0f },
	{ 0.0f, 0.0f, 0.0f, 1.0f }
}; // ĳ���� ��ġ

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
		{  -0.5f, 0.0f, 0.0f,  0xffff0000 },
		{   0.0f, 0.5f, 0.0f,  0xff00ff00 },
		{   0.5f, 0.0f, 0.0f,  0xff00ffff },

		{  -0.5f, 0.0f, 0.0f,  0xffff0000 },
		{   0.0f,-0.5f, 0.0f,  0xff00ff00 },
		{   0.5f, 0.0f, 0.0f,  0xff00ffff },
	};

	if (FAILED(g_pVooDooDevice->CreateVertexBuffer(sizeof(vertices), 0, FVF_COLORVERTEX, VDPOOL_DEFAULT, &g_pVertexBuffer, nullptr))) { return -1; }

	COLORVERTEX* pBuffer = nullptr;
	if (FAILED(g_pVertexBuffer->Lock(0, sizeof(vertices), (void**)&pBuffer, 0))) { return -2; }

	memcpy(pBuffer, vertices, sizeof(vertices));

	g_pVertexBuffer->Unlock();

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
	if (IsKeyDown(VK_LEFT)) characterLocationMatrix[3][0] -= speed * deltaTime; // ���� �̵�
	if (IsKeyDown(VK_RIGHT)) characterLocationMatrix[3][0] += speed * deltaTime; // ������ �̵�
	if (IsKeyDown(VK_UP)) characterLocationMatrix[3][2] += speed * deltaTime; // ���� �̵�
	if (IsKeyDown(VK_DOWN)) characterLocationMatrix[3][2] -= speed * deltaTime; // ���� �̵�

	float rotationXMatrix[4][4] = 
	{
		{ cosf(angle), -sinf(angle), 0.0f, 0.0f },
		{ sinf(angle), cosf(angle), 0.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	}; // x�� ȸ�� ���
	float rotationZMatrix[4][4] = 
	{
		{ cosf(angle), 0.0f, sinf(angle), 0.0f },
		{ 0.0f, 1.0f, 0.0f, 0.0f },
		{ -sinf(angle), 0.0f, cosf(angle), 0.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	}; // z�� ȸ�� ���

	VDMATRIX rotationX(true, rotationXMatrix);
	VDMATRIX rotationZ(true, rotationZMatrix);
	VDMATRIX translation(false, characterLocationMatrix);

	rotationZ *= translation;
	rotationZ.Transposition(); // ��� ��ġ
	rotationX *= rotationZ; // Z�� ȸ�� �� X�� ȸ��

	g_mTM = rotationX; // ȸ�� ����� ���� ��ȯ ��Ŀ� ����
}

void CameraUpdate(float deltaTime)
{
	VDVECTOR cameraPosition(0.0f, 10.0f, -15.0f);
	VDVECTOR cameraTarget(characterLocationMatrix[3][0], 0.0f, characterLocationMatrix[3][2]);
	VDVECTOR cameraUp(0.0f, 1.0f, 0.0f);

	g_mView.VDMatrixLookAtLH(cameraPosition, cameraTarget, cameraUp); // ī�޶� �� ��� ����
	g_mView.Transposition(); // ��� ��ġ
	g_pVooDooDevice->SetTransform(VDTS_VIEW, &g_mView); // �� ��� ����

	float fov = rad(45.0f);
	float aspectRatio = static_cast<float>(g_displayMode.Width) / static_cast<float>(g_displayMode.Height);
	float nearPlane = 0.1f;
	float farPlane = 100.0f;

	g_mProj.VDMatrixPerspectiveFovLH(fov, aspectRatio, nearPlane, farPlane); // ���� ���� ��� ����
	g_mProj.Transposition(); // ��� ��ġ
	g_pVooDooDevice->SetTransform(VDTS_PROJECTION, &g_mProj); // ���� ��� ����
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
	g_pVooDooDevice->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(COLORVERTEX));
	g_pVooDooDevice->SetFVF(FVF_COLORVERTEX);

	float translationMatrix0[4][4] =
	{
		{ 1.0f, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f, 0.0f },
		{ 3.0f, 0.0f, 0.0f, 1.0f }
	};
	float translationMatrix1[4][4] =
	{
		{ 1.0f, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f, 0.0f },
		{ -3.0f, 0.0f, 0.0f, 1.0f }
	};
	float translationMatrix2[4][4] =
	{
		{ 1.0f, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 3.0f, 1.0f }
	};
	float translationMatrix3[4][4] =
	{
		{ 1.0f, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, -3.0f, 1.0f }
	};

	for (int i = 0; i < 4; ++i)
	{
		VDMATRIX translationMatrix(false, (i == 0) ? translationMatrix0 :
			(i == 1) ? translationMatrix1 :
			(i == 2) ? translationMatrix2 : translationMatrix3);
		translationMatrix.Transposition(); // ��� ��ġ
		g_pVooDooDevice->SetTransform(VDTS_WORLD, &translationMatrix); // ���� ��� ����
		g_pVooDooDevice->DrawPrimitive(VDPT_TRIANGLELIST, 0, 2);
	}
}

void SystemUpdate()
{
	if (IsKeyUp(VK_ESCAPE)) g_bWireframe = !g_bWireframe;

	g_pVooDooDevice->SetRenderState(VDRS_FILLMODE, g_bWireframe ? VDFILL_WIREFRAME : VDFILL_SOLID);
	g_pVooDooDevice->SetRenderState(VDRS_CULLMODE, VDCULL_NONE);

	if (g_bWireframe) g_ClearColor = VDCOLOR(1.0f, 1.0f, 0.0f, 1.0f);
	else g_ClearColor = VDCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	if (IsKeyUp(VK_F1)) g_bShowGrid = !g_bShowGrid;
	if (IsKeyUp(VK_F2)) g_bShowAxis = !g_bShowAxis;
	g_pVooDooDevice->SetRenderState(VDRS_GRIDMODE, g_bShowGrid ? TRUE : FALSE);
	g_pVooDooDevice->SetRenderState(VDRS_AXISMODE, g_bShowAxis ? TRUE : FALSE);
}

void SceneRender()
{
	SystemUpdate();

	float deltaTime = GetEngineTime();
	ObjUpdate(deltaTime);
	CameraUpdate(deltaTime);

	g_pVooDooDevice->BeginScene();
	g_pVooDooDevice->Clear(g_ClearColor);

	g_pVooDooDevice->DrawGrid();

	//ShowFPS(1, 1); // �޸� ���� ����

	DummyDraw();
	ObjDraw();
	
	g_pVooDooDevice->EndScene();
	g_pVooDooDevice->Present();
}