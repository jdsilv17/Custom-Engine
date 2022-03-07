#pragma once

#include "Resource.h"
#include "framework.h"
#include "Graphics.h"

#define MAX_LOADSTRING 100

class WindowContainer
{
public:
	ATOM	MyRegisterClass(HINSTANCE hInstance);
	BOOL	InitInstance(HINSTANCE, int);
	LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

protected:
	Graphics GFX;

	HINSTANCE       hInst;                                // current instance
	HWND            hWnd = nullptr;
	WCHAR           szTitle[MAX_LOADSTRING];                  // The title bar text
	WCHAR           szWindowClass[MAX_LOADSTRING];            // the main window class name
};

