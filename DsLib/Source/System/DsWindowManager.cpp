#include "DsPch.h"
#ifndef _DS_WINDOW_MANAGER_H_
#include "System/DsWindowManager.h"
#endif

#ifndef _DS_WINDOW_H_
#include "System/DsWindow.h"
#endif
#ifndef _DS_WINDOW_EZ_H_
#include "System/DsWindowEz.h"
#endif
#ifndef _DS_WINDOW_GL_H_
#include "System/DsWindowGL.h"
#endif
#ifndef _DS_SYS_H_
#include "System/DsSys.h"
#endif
#ifndef _DS_MOUSE_H_
#include "Mouse/DsMouse.h"
#endif

#include<windows.h>

using namespace DsLib;

DsWindowManager::DsWindowManager()
{
}

DsWindowManager::~DsWindowManager()
{
}


namespace
{
	static DsSys* s_pSys = NULL;
	static bool s_isOneFrame = true;

	int _GetDsMouseState(UINT msg)
	{
		switch (msg) {
		case WM_LBUTTONDOWN: return DsMouseClickState::DS_LEFT_CLICK;
		case WM_MBUTTONDOWN: return DsMouseClickState::DS_CENTER_CLICK;
		case WM_RBUTTONDOWN: return DsMouseClickState::DS_RIGHT_CLICK;
		default: DS_ASSERT(false, "意図しないマウスステート[%d]", msg);
		}
		return 0;
	}

	LRESULT CALLBACK WindowProc(HWND hWindow, UINT msg, WPARAM wp, LPARAM lp)
	{
		switch (msg) {
			case WM_CREATE:
			{
				SetTimer(hWindow, 1, 1000, NULL);
			}
			break;

			case WM_PAINT:
			{
				if (s_pSys) {
					if (s_isOneFrame) {
						s_pSys->RefWindow().Reshape();
						s_pSys->RefWindow().Display();
						s_isOneFrame = false;
					}
				}
			}
			break;

			case WM_SIZE:
			{
				if (s_pSys) {
					s_pSys->RefWindow().Reshape();
				}
			}
			break;

			case WM_MOUSEMOVE:
			{
				if (s_pSys) {
					int x = LOWORD(lp);
					int y = HIWORD(lp);
					s_pSys->RefWindow().Drag(x, y);
				}
			}
			break;

			case WM_MOUSEWHEEL:
			{
				if (s_pSys) {
					int zDelta = GET_WHEEL_DELTA_WPARAM(wp);
					int x = LOWORD(lp);
					int y = HIWORD(lp);
					s_pSys->RefWindow().Wheel(zDelta / WHEEL_DELTA, x, y);
				}
			}
			break;

			case WM_LBUTTONDOWN:
			case WM_MBUTTONDOWN:
			case WM_RBUTTONDOWN:
			{
				if (s_pSys) {
					int x = LOWORD(lp);
					int y = HIWORD(lp);
					s_pSys->RefWindow().Click(_GetDsMouseState(msg), DsMouseUpDown::DS_DOWN_MOUSE, x, y);
				}
			}
			break;

			case WM_LBUTTONUP:
			case WM_MBUTTONUP:
			case WM_RBUTTONUP:
			{
				if (s_pSys) {
					int x = LOWORD(lp);
					int y = HIWORD(lp);
					s_pSys->RefWindow().Click(DsMouseClickState::DS_NON_CLICK, DsMouseUpDown::DS_UP_MOUSE, x, y);
				}
			}
			break;

			case WM_KEYDOWN:
			{
				if (s_pSys) {
					int key = GET_KEYSTATE_WPARAM(wp);
					s_pSys->RefWindow().Keyboard(key, 0, 0);
				}
			}
			break;

			case WM_KEYUP:
			{
				if (s_pSys) {
					int key = GET_KEYSTATE_WPARAM(wp);
					s_pSys->RefWindow().KeyboardUp(key, 0, 0);
				}
			}
			break;

			case WM_TIMER:
			{
				InvalidateRect(hWindow, 0, false);
			}
			break;

			case WM_DESTROY:
			{
				PostQuitMessage(0);
				exit(0);
			}
			break;

			default:
			{
				return DefWindowProc(hWindow, msg, wp, lp);
			}
		}
		return 0;
	}
}

/*
return 作成したウィンドウハンドル(HWND)
*/
//static 
ds_uint64 DsWindowManager::MainWindowCreate(ds_uint64 _hInstance, char* lpCmdLine, int nCmdShow)
{
	HINSTANCE hInstance = (HINSTANCE)_hInstance;

	WNDCLASS wc;
	/*ウィンドウクラスの登録*/
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = NULL;
	wc.hbrBackground = (HBRUSH)COLOR_BACKGROUND + 1;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = L"MainWindow";

	if (!RegisterClass(&wc))
	{
		MessageBox(NULL, TEXT("ウィンドウクラスの作成に失敗しました"), NULL, MB_OK);
		return 0;
	}

	HWND hwnd = CreateWindowEx(
		0,                              // オプションのウィンドウ スタイル
		L"MainWindow",                     // ウィンドウ クラス
		L"DsApplication",    // ウィンドウ テキスト
		WS_OVERLAPPEDWINDOW,            // ウィンドウ スタイル

										// サイズと位置
		CW_USEDEFAULT, CW_USEDEFAULT, DsWindow::INIT_WINDOW_W, DsWindow::INIT_WINDOW_H,

		NULL,       // 親ウィンドウ    
		NULL,       // メニュー
		hInstance,  // インスタンス ハンドル
		NULL        // 追加のアプリケーション データ
	);
	if (NULL == hwnd) {
		MessageBox(NULL, TEXT("ウィンドウクラスの作成に失敗しました"), NULL, MB_OK);
		return 0;
	}
	ShowWindow(hwnd, nCmdShow);

	return (ds_uint64)hwnd;
}

/*
return 終了したか
*/
//static
bool DsWindowManager::MainWindowLoop(ds_uint64 _hwnd, DsSys& sys)
{
	s_pSys = &sys;

	LARGE_INTEGER freq;
	bool isEnableTimer = QueryPerformanceFrequency(&freq);

	while (1) {
		HWND hwnd = (HWND)_hwnd;

		LARGE_INTEGER start;
		if (isEnableTimer && QueryPerformanceCounter(&start) ) {
			LONGLONG curTimeMs = static_cast<LONGLONG>((static_cast<double>(start.QuadPart) / static_cast<double>(freq.QuadPart))*1000.0);
			if ( (curTimeMs % 16) == 0) {
				s_isOneFrame = true;
				InvalidateRect(hwnd, 0, false);
			}
		}
		
		MSG msg;
		if (PeekMessage(&msg, hwnd, 0, 0, PM_NOREMOVE)) {
			int mesRet = GetMessage(&msg, hwnd, 0, 0);
			if (mesRet != 0) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else {
				break;
			}
		}
	}
	return true;
}

//static 
DsWindow* DsWindowManager::Create(DS_WINDOW_SYSTEM_TYPE type)
{
	DsWindow* pWindow=0;

	switch (type)
	{
	case DS_WINDOW_SYSTEM_TYPE_GL:
		{
			pWindow = new DsWindowGL();
		}
		break;
	case DS_WINDOW_SYSTEM_TYPE_GLUT:
		{
			//pWindow = new DsWindowEz();
			DS_ASSERT(false, "DsWindowEzは廃止されました");
		}
		break;

	default:
		pWindow = new DsWindow();
		break;
	}

	return pWindow;
}