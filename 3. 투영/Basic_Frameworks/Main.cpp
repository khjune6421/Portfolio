#include "Render.h"
#include "Device.h"

HWND g_hWnd = nullptr;
BOOL g_bLoop = TRUE;
HINSTANCE g_hInstance = nullptr;

const WCHAR* g_szClassName = L"VooDooClass";
const WCHAR* g_windowName = L"VooDoo Device Test";

BOOL InitWindows(int with, int height);
void ResetWindows(HWND hWnd, UINT width, UINT height);
BOOL MessagePump();
void MoveToCenter(HWND hWnd, int width, int height);

LRESULT CALLBACK MsgProc(HWND, UINT, WPARAM, LPARAM);

int main()
{
	if (!InitWindows(800, 600)) { return -1; };
	if (!VooDooSetup(g_hWnd)) { return -2; }
	if (!DataLoading()) { return -3; }

	while (g_bLoop)
	{
		if (!MessagePump()) { break; }

		SceneRender();
	}
	DataRelease();
	VooDooRelease();

	return 0;
}

LRESULT CALLBACK MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			SendMessage(hWnd, WM_DESTROY, 0, 0);
			break;
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}

BOOL InitWindows(int width, int height)
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MsgProc;
	wc.hInstance = g_hInstance;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.lpszClassName = g_szClassName;
	if (!RegisterClassEx(&wc)) { return FALSE; }
	g_hWnd = CreateWindowEx
	(
		0,
		g_szClassName,
		g_windowName,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, width, height,
		nullptr, nullptr, g_hInstance, nullptr
	);
	if (!g_hWnd) { return FALSE; }
	ShowWindow(g_hWnd, SW_SHOW);
	UpdateWindow(g_hWnd);
	return TRUE;
}

int MessagePump()
{
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			g_bLoop = FALSE;
			return 0;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 1;
}

void ResetWindows(HWND hWnd, UINT width, UINT height)
{
	RECT rect = { 0, 0, (LONG)width, (LONG)height };
	AdjustWindowRect(&rect, GetWindowLong(hWnd, GWL_STYLE), FALSE);
	SetWindowPos(hWnd, nullptr, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);
	MoveToCenter(hWnd, width, height);
}

void MoveToCenter(HWND hWnd, int width, int height)
{
	RECT rect;
	GetWindowRect(hWnd, &rect);
	int x = (GetSystemMetrics(SM_CXSCREEN) - (rect.right - rect.left)) / 2;
	int y = (GetSystemMetrics(SM_CYSCREEN) - (rect.bottom - rect.top)) / 2;
	SetWindowPos(hWnd, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}