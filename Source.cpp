#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <windows.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

TCHAR szClassName[] = TEXT("Window");

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("ウィンドウがアクティブのとき Win キーを無効にする (DirectInput版)"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);

	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	static LPDIRECTINPUTDEVICE8  lpdiKeyboard;
	static IDirectInput8 *directInput;

	if (SUCCEEDED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, NULL)))
	{
		if (SUCCEEDED(directInput->CreateDevice(GUID_SysKeyboard, &lpdiKeyboard, NULL)))
		{
			if (SUCCEEDED(lpdiKeyboard->SetDataFormat(&c_dfDIKeyboard)))
			{
				if (SUCCEEDED(lpdiKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY)))
				{
					lpdiKeyboard->Acquire();
				}
			}
		}
	}

	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (FAILED(lpdiKeyboard->GetDeviceState(0, 0)))
		{
			lpdiKeyboard->Acquire();
			lpdiKeyboard->GetDeviceState(0, 0);
		}
	}

	lpdiKeyboard->Release();
	directInput->Release();
	CoUninitialize();

	return (int)msg.wParam;
}
