// WinMain.cpp

#include "stdafx.h"
#include "resource.h"
#include "Config.h"
#include "Engine.h"

//#include "RenderSystem.h"
//#include "RenderResources.h"
//#include "Camera.h"

//bool keys[6];
//int mouse[2];
//bool button[2];

// -- helper functions --------------------------------------------------------
HWND CreateRenderWindow(wchar_t *class_name, HINSTANCE inst, int width, int height);
void DestroyRenderWindow(wchar_t *class_name, HINSTANCE inst, HWND hWnd);

// -- message callback -------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// -- win .... main! ---------------------------------------------------------
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
	Config config("../data/config.cfg");
	if(!config.Read()) {
		Debug::Msg("Could not find '../data/config.cfg'!\nExiting...");
		return -1;
	};

	wchar_t *kClassName = L"kOpWndClass";
	int width = config.Get("width", 1024);
	int height = config.Get("height", 640);

	HWND hWnd = CreateRenderWindow(kClassName, hInstance, width, height);
	SetWindowText(hWnd, L"GPIIWindowDX");

	ShowWindow(hWnd, nCmdShow);
	ShowWindow(hWnd, SW_SHOWDEFAULT);

	Engine engine;
	if(!engine.Initialize(config, hWnd)) {
		Debug::Msg("Could not initialize engine!");
		return -1;
	};

	SetWindowLongPtrA(hWnd, GWLP_USERDATA, (LONG)&engine);

	Timer timer;
	MSG msg;
	bool running = true;
	while(running) {
		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if(msg.message == WM_QUIT) { running = false; };
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		};
		running = running && engine.Update( timer.Update() );
		::Sleep(10);
	};

	engine.Cleanup();
	DestroyRenderWindow(kClassName, hInstance, hWnd);

	return 0;
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	Engine* engine = (Engine*)GetWindowLongPtrA(hWnd, GWLP_USERDATA);
	if(!engine) return DefWindowProc(hWnd, message, wParam, lParam);

	switch(message) {
	case WM_MOUSEMOVE:
		engine->OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;

	case WM_LBUTTONDOWN: 
		engine->OnMouseButton(0, true);
		break;
	case WM_RBUTTONDOWN: 
		engine->OnMouseButton(1, true);
		break;
	case WM_LBUTTONUP: 
		engine->OnMouseButton(0, false);
		break;
	case WM_RBUTTONUP: 
		engine->OnMouseButton(1, false);
		break;

	case WM_KEYDOWN:
		engine->OnKeyboard(wParam, true);
		break;
	case WM_KEYUP:
		engine->OnKeyboard(wParam, true);
		break;
	
	case WM_SETFOCUS:
		engine->OnFocus(true);
		break;
	case WM_KILLFOCUS:
		engine->OnFocus(false);
		break;
	
	case WM_CLOSE:
		engine->OnQuit();
		::PostQuitMessage(0);
	break;
		default: return ::DefWindowProc(hWnd, message, wParam, lParam);	
	};
	return 0;
};

HWND CreateRenderWindow(wchar_t *class_name, HINSTANCE inst, int width, int height) {
	RECT wr = {0, 0, width, height};
	DWORD style = WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME|WS_MAXIMIZEBOX|WS_MINIMIZEBOX);

	WNDCLASSEX wc = {0};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = class_name;
	wc.style = CS_HREDRAW | CS_OWNDC | CS_VREDRAW;
	wc.hCursor = LoadCursor(NULL,IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.hIcon = LoadIcon(inst, MAKEINTRESOURCE(IDI_GPIIBASE));
	wc.hIconSm = LoadIcon(inst, MAKEINTRESOURCE(IDI_SMALL));
	RegisterClassEx(&wc);
	AdjustWindowRect(&wr, style, FALSE);

	HWND hWnd = CreateWindowEx(0,
			class_name, NULL,
			style, CW_USEDEFAULT,CW_USEDEFAULT,
			wr.right-wr.left, wr.bottom-wr.top,
			NULL, NULL, wc.hInstance, NULL);
	if(hWnd == NULL) { return NULL; };

	return hWnd;
};

void DestroyRenderWindow(wchar_t *class_name, HINSTANCE inst, HWND hWnd) {
	DestroyWindow(hWnd);
	UnregisterClass(class_name, inst);
};
