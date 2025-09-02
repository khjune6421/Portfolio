#pragma once
#include "VooDoo.h"

int VooDooSetup(HWND hWnd);
void VooDooRelease();

void ShowFPS(int x, int y);
float GetEngineTime();

void DrawText(int x, int y, COLORREF color, const WCHAR* msg, ...);

extern BOOL g_showFPS;
extern HWND g_hWnd;
extern VDDISPLAYMODE g_displayMode;
extern IVooDooDevice* g_pVooDooDevice;

#define IsKeyDown(key) (GetAsyncKeyState(key) & 0x8000)
#define IsKeyUp(key) (GetAsyncKeyState(key) & 0x8001)