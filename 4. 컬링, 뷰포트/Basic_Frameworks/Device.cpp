#include "Device.h"
#include <iostream>

IVooDoo* g_pVooDoo = nullptr;
IVooDooDevice* g_pVooDooDevice = nullptr;

VDDISPLAYMODE g_displayMode = { 1600, 600 };

HFONT g_hFont = nullptr;
COLORREF g_fontColor = RGB(0, 0, 0);

BOOL g_showFPS = FALSE;

int VooDooSetup(HWND hWnd)
{
	g_pVooDoo = VooDooCreate(0);
	if (!g_pVooDoo) { return -1; }

	VDPRESENT_PARAMETERS pp;
	ZeroMemory(&pp, sizeof(pp));

	pp.BackBufferWidth = g_displayMode.Width;
	pp.BackBufferHeight = g_displayMode.Height;
	pp.BackBufferCount = 1;
	pp.Windowed = TRUE;

	g_pVooDoo->CreateDevice(g_hWnd, &pp, VDCREATE_SOFTWARE_VERTEXPROCESSING, &g_pVooDooDevice);
	if (!g_pVooDooDevice) { return -2; }

	g_hFont = CreateFont
	(
		12, 6,
		0, 0, 1, 0, 0, 0,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		FF_DECORATIVE,
		L"굴림"
	);

	return 1;
}

void VooDooRelease()
{
	DeleteObject(g_hFont);

	if (g_pVooDooDevice)
	{
		g_pVooDooDevice->Release();
		g_pVooDooDevice = nullptr;
	}
	if (g_pVooDoo)
	{
		g_pVooDoo->Release();
		g_pVooDoo = nullptr;
	}
}

void DrawText(int x, int y, COLORREF col, const WCHAR* msg, ...)
{
	WCHAR buffer[32];
	va_list args;

	va_start(args, msg);
	vswprintf_s(buffer, msg, args);
	va_end(args);

	RECT rect = { x, y, x + g_displayMode.Width, y + g_displayMode.Height };

	HDC hdc = GetDC(g_hWnd);
	SetTextColor(hdc, col);
	DrawText(hdc, buffer, static_cast<int>(wcslen(buffer)), &rect, DT_WORDBREAK);
	ReleaseDC(g_hWnd, hdc);
}

void ShowFPS(int x, int y)
{
	static UINT frame = 0;
	static float framesPerSecond = 0.0f;
	++frame;

	static ULONGLONG lastTime = GetTickCount64();
	ULONGLONG currentTime = GetTickCount64();

	UINT timeElapsed = static_cast<UINT>(currentTime - lastTime);
	if (timeElapsed >= 1000)
	{
		framesPerSecond = static_cast<float>(frame) / (static_cast<float>(timeElapsed) / 1000.0f);
		frame = 0;
		lastTime = currentTime;
	}
	DrawText(x, y, g_fontColor, L"FPS: %.0f", framesPerSecond);
}

float GetEngineTime()
{
	static ULONGLONG lastTime = GetTickCount64();
	ULONGLONG currentTime = GetTickCount64();

	float deltaTime = static_cast<float>(currentTime - lastTime) / 1000.0f; // 초 단위로 변환
	lastTime = currentTime;

	return deltaTime;
}