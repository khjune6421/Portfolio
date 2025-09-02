#pragma once
#include <Windows.h>

int ObjLoad();
void ObjRelease();
void ObjUpdate(float deltaTime);
void CameraUpdate(float deltaTime, bool isOrthographic = false);

int DataLoading();
void DataRelease();
void SceneRender();
float GetEngineTime();

extern HWND g_hWnd;