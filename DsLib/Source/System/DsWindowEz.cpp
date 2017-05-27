#include "DsPch.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "gl/freeglut.h"

#ifndef _DS_WINDOW_EZ_H_
#include "System/DsWindowEz.h"
#endif

#include "Graphics/Camera/DsCamera.h"
#include "Graphics/Render/DsRender.h"
#include "System/DsMainLoop.h"
#include "System/DsSys.h"
#include "Mouse/DsMouse.h"
#ifndef _DS_KEYBOARD_H_
#include "Keyboard/DsKeyboard.h"
#endif
#ifndef _DS_WINDOW_MANAGER_H_
#include "System/DsWindowManager.h"
#endif

using namespace DsLib;

static DsWindowEz* s_pWin = 0;

DsWindowEz::DsWindowEz()
{
}

DsWindowEz::~DsWindowEz()
{
	s_pWin = NULL;
}

void DsWindowEz::Initialize(InitArgs& args)
{
	DsWindow::Initialize(args);

	glutInit( &args.argc, args.argv );
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(GetWindowW(), GetWindowH());
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow("");
	glClearColor(0.7f, 0.8f, 1.0f, 1);
}

void DsWindowEz::Setup()
{
	DsWindow::Setup();

	s_pWin = this;
	glutDisplayFunc(_Display);
	glutKeyboardFunc(_Keyboard);
	glutKeyboardUpFunc(_KeyboardUp);
	glutSpecialFunc(_KeyboardSp);
	//glutPassiveMotionFunc(_MouseMotion);
	glutMotionFunc(_Drag);
	glutMouseFunc(_Click);
	glutMouseWheelFunc(_Wheel);
	glutTimerFunc(1000, _Timer, 0);
	//glutIdleFunc(Idle);
	glutReshapeFunc(_Reshape);
	glutMainLoop();
}

//virtual 
int DsWindowEz::GetCurrentWindowW() const
{
	return glutGet(GLUT_WINDOW_WIDTH);
}
//virtual 
int DsWindowEz::GetCurrentWindowH() const
{
	return glutGet(GLUT_WINDOW_HEIGHT);
}

//static
void DsWindowEz::_Display()
{

	DsMainLoopArgs args(
		s_pWin->m_pSys->RefRender().RefDrawCom(),
		s_pWin->m_pSys->RefRender().GetRenderTool(),
		*(s_pWin->m_pSys),
		s_pWin->m_pSys->GetDt());
	s_pWin->m_pLoop->BeforeWindowUpdate(args);
	s_pWin->m_pSys->Update();
	s_pWin->m_pLoop->AfterWindowUpdate();
	glutSwapBuffers();
}

//static
void DsWindowEz::_Timer( int value )
{
	glutPostRedisplay();
	unsigned int dt = static_cast<unsigned int>(s_pWin->GetDt()*1000.0);
	glutTimerFunc(dt, _Timer, 0);
}

//static
void DsWindowEz::_Reshape( int w, int h )
{
	const DsCamera* pCam = s_pWin->m_pCam;
	
	const double zfar = pCam->GetFar();
	const double znear = pCam->GetNear();
	const double fovy = pCam->GetFovy();
	
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, static_cast<double>(w) / static_cast<double>(h), znear, zfar);
	glMatrixMode(GL_MODELVIEW);

	s_pWin->m_windowW = w;
	s_pWin->m_windowH = h;
	s_pWin->m_pCam->SetAspect(static_cast<float>(w) / static_cast<float>(h));
}

//static
void DsWindowEz::_Drag(int x, int y)
{
	s_pWin->m_pMouse->OnDrag(DsVec2i(x, y));
}

//static
void DsWindowEz::_Click(int button, int state, int x, int y)
{
	DsMouseClickState b = DS_NON_CLICK;
	switch (button)
	{
	case GLUT_RIGHT_BUTTON:
		b = DS_RIGHT_CLICK;
		break;
	case GLUT_LEFT_BUTTON:
		b = DS_LEFT_CLICK;
		break;
	case GLUT_MIDDLE_BUTTON:
		b = DS_CENTER_CLICK;
		break;
	default:
		break;
	}

	DsMouseUpDown ud = DS_DOWN_MOUSE;
	if (GLUT_DOWN == state)
	{
		ud = DS_DOWN_MOUSE;
	}
	else
	{
		ud = DS_UP_MOUSE;
	}
	s_pWin->m_pMouse->OnClick(b, ud, x, y);
}

//static 
void DsWindowEz::_Wheel(int wheel, int direction, int x, int y)
{
	s_pWin->m_pMouse->OnWheel(direction, x, y);
}

//static 
void DsWindowEz::_Keyboard(unsigned char key, int x, int y)
{
	s_pWin->m_pKeyboard->OnKey(key, true);
}

//static 
void DsWindowEz::_KeyboardUp(unsigned char key, int x, int y)
{
	s_pWin->m_pKeyboard->OnKey(key, false);
}

//static 
void DsWindowEz::_KeyboardSp(int key, int x, int y)
{
	s_pWin->m_pKeyboard->OnKey(key, true);
}

//virtual 
DsMat33d  DsWindowEz::GetPerspective() const
{
	double tmp[16];
	glGetDoublev(GL_PROJECTION_MATRIX, tmp);
	return DsMat33d::ToMat33(tmp);
}