#include "DsPch.h"

#ifndef _DS_WINDOW_GL_H_
#include "System/DsWindowGL.h"
#endif

#include  <GL/gl.h>
#include  <GL/glu.h>

#include "Graphics/Camera/DsCamera.h"
#include "Graphics/Render/DsRender.h"
#include "System/DsMainLoop.h"
#include "System/DsSys.h"
#include "Mouse/DsMouse.h"
#ifndef _DS_KEYBOARD_H_
#include "Keyboard/DsKeyboard.h"
#endif

using namespace DsLib;

DsWindowGL::DsWindowGL()
	:m_hWnd(0)
	,m_hDC(0)
	,m_hGLRC(0)
{

}

DsWindowGL::~DsWindowGL()
{

}

void DsWindowGL::Initialize(InitArgs& args)
{
	DsWindow::Initialize(args);

	_SetHandle(args.handle);
	glClearColor(0.7f, 0.8f, 1.0f, 1);
}

//virtual 
void DsWindowGL::Finalize()
{
}

void DsWindowGL::Setup()
{
	DsWindow::Setup();
}

//virtual 
void DsWindowGL::SetHandle(int handle)
{
	_SetHandle(handle);
}

//virtual 
int DsWindowGL::GetCurrentWindowW() const
{
	RECT rc;
	GetClientRect((HWND)m_hWnd, &rc);
	return (rc.right - rc.left);
}
//virtual 
int DsWindowGL::GetCurrentWindowH() const
{
	RECT rc;
	GetClientRect((HWND)m_hWnd, &rc);
	return (rc.bottom - rc.top);
}

void DsWindowGL::Display()
{
	DsMainLoopArgs args(
		m_pSys->RefRender().RefDrawCom(),
		m_pSys->RefRender().GetRenderTool(),
		*(m_pSys),
		m_pSys->GetDt()
		);
	wglMakeCurrent((HDC)m_hDC, (HGLRC)m_hGLRC);
	m_pLoop->BeforeWindowUpdate(args);
	m_pSys->Update();
	m_pLoop->AfterWindowUpdate();
	SwapBuffers((HDC)m_hDC);
	wglMakeCurrent((HDC)m_hDC, 0);
}

void DsWindowGL::Timer(int value)
{
	//glutPostRedisplay();
	//glutTimerFunc(static_cast<unsigned int>(GetDt()), Timer, 0);
}

void DsWindowGL::Reshape()
{
	const int w = GetCurrentWindowW();
	const int h = GetCurrentWindowH();
	const DsCamera* pCam = m_pCam;

	const double zfar = pCam->GetFar();
	const double znear = pCam->GetNear();
	const double fovy = pCam->GetFovy();

	wglMakeCurrent((HDC)m_hDC, (HGLRC)m_hGLRC);
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, static_cast<double>(w) / static_cast<double>(h), znear, zfar);
	glMatrixMode(GL_MODELVIEW);

	m_windowW = w;
	m_windowH = h;
	m_pCam->SetAspect(static_cast<float>(w) / static_cast<float>(h));
	wglMakeCurrent((HDC)m_hDC, 0);
}

void DsWindowGL::Drag(int x, int y)
{
	if ((m_pMouse->GetClickState() == DS_LEFT_CLICK) || (m_pMouse->GetClickState() == DS_RIGHT_CLICK) || (m_pMouse->GetClickState() == DS_CENTER_CLICK))
	{
		m_pMouse->OnDrag(DsVec2i(x, y));
	}
}

void DsWindowGL::Click(int button, int state, int x, int y)
{
	m_pMouse->OnClick(static_cast<DsMouseClickState>(button), static_cast<DsMouseUpDown>(state), x, y);
}

void DsWindowGL::Wheel(int wheel, int x, int y)
{
	m_pMouse->OnWheel(wheel, x, y);
}

void DsWindowGL::Keyboard(unsigned char key, int x, int y)
{
	m_pKeyboard->OnKey(key, true);
}

void DsWindowGL::KeyboardUp(unsigned char key, int x, int y)
{
	m_pKeyboard->OnKey(key, false);
}

void DsWindowGL::KeyboardSp(int key, int x, int y)
{
	m_pKeyboard->OnKey(key, true);
}

void DsWindowGL::_SetHandle(int handle)
{
	if (m_hDC != 0)
	{
		wglMakeCurrent((HDC)m_hDC, 0);
	}

	m_hWnd = handle;
	m_hDC = (int)GetDC((HWND)m_hWnd);

	static const PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),     // 構造体のサイズ
		1,                                  // OpenGL バージョン
		PFD_DRAW_TO_WINDOW |                // ウィンドウスタイル
		PFD_SUPPORT_OPENGL |                // OpenGL を使う
		PFD_DOUBLEBUFFER,                   // ダブルバッファ
		PFD_TYPE_RGBA,                      // ピクセルのカラーデータ
		32,                                 // 色のビット数
		0, 0, 0, 0, 0, 0, 0, 0,             // RGBAカラーバッファのビット
		0, 0, 0, 0, 0,                      // アキュムレーションバッファのピクセル当りのビット数
		32,                                 // デプスバッファ    のピクセル当りのビット数
		0,                                  // ステンシルバッファのピクセル当りのビット数
		0,                                  // 補助バッファ      のピクセル当りのビット数
		PFD_MAIN_PLANE,                     // レイヤータイプ
		0,                                  // 
		0,                                  // 
		0,                                  // 
		0                                   // 
	};

	HDC hdc = (HDC)m_hDC;
	int pixelformat;
	pixelformat = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, pixelformat, &pfd);
	m_hGLRC = (int)wglCreateContext(hdc);

	//カレントを設定しておかないとDsRenderの初期化のgl系の初期化がうまくいかない
	wglMakeCurrent((HDC)m_hDC, (HGLRC)m_hGLRC);
}

//virtual 
DsMat33d  DsWindowGL::GetPerspective() const
{
	double tmp[16];
	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(m_pCam->GetFovy(), m_pCam->GetAspect(), m_pCam->GetNear(), m_pCam->GetFar());
	glGetDoublev(GL_PROJECTION_MATRIX, tmp);
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	return DsMat33d::ToMat33(tmp);
}