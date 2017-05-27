#include "DsPch.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "gl/glut.h"
#include "System/DsEzWindow.h"
#include "Graphics/Camera/DsCamera.h"
#include "Graphics/Render/DsRender.h"
#include "System/DsMainLoop.h"
#include "System/DsSys.h"

using namespace DsLib;

DsEzWindow::DsEzWindow()
:m_windowW(INIT_WINDOW_W)
,m_windowH(INIT_WINDOW_H)
,m_mspf(1000.0f/(float)(DEFAULT_FPS))
,m_pCam(NULL)
,m_pLoop(NULL)
,m_pSys(NULL)
{
}

DsEzWindow::~DsEzWindow()
{
}

void DsEzWindow::Initialize(InitArgs& args)
{
	if (NULL == args.pCam)
	{
		DsError::Panic("");
	}
	if (NULL == args.pLoop)
	{
		DsError::Panic("");
	}
	if (NULL == args.pSys)
	{
		DsError::Panic("");
	}
	m_pCam = args.pCam;
	m_pLoop = args.pLoop;
	m_pSys = args.pSys;
	glutInit( &args.argc, args.argv );
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(GetWindowW(), GetWindowH());
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow("");
	glClearColor(0.7, 0.8, 1.0, 1);
}

void DsEzWindow::SetUpCallBack()
{
	glutDisplayFunc(_Display);
	//glutKeyboardFunc(Keyboard);
	//glutKeyboardUpFunc(KeyboardUp);
	//glutPassiveMotionFunc(_MouseMotion);
	glutMotionFunc(_Drag);
	//glutMouseFunc(_MouseClick);
	glutTimerFunc(1000, _Timer, 0);
	//glutIdleFunc(Idle);
	glutReshapeFunc(_Reshape);

	glutMainLoop();
}

//static
void DsEzWindow::_Display()
{
	DsMainLoopUpdateArgs args(
		GetIns().m_pSys->RefRender().RefDrawCom(),
		GetIns().m_pSys->RefRender().GetRenderTool(),
		GetIns().m_pSys->GetDt() );
	GetIns().m_pLoop->BeforeWindowUpdate(args);
	glutSwapBuffers();
	GetIns().m_pSys->Update();
	GetIns().m_pLoop->AfterWindowUpdate();
}

//static
void DsEzWindow::_Timer( int value )
{
	glutPostRedisplay();
	glutTimerFunc(GetIns().GetDt(), _Timer, 0);
}

//static
void DsEzWindow::_Reshape( int w, int h )
{
	const DsCamera* pCam = GetIns().m_pCam;
	
	const double zfar = pCam->GetFar();
	const double znear = pCam->GetNear();
	const double fovy = pCam->GetFovy();
	
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy,(double)w/h,znear,zfar);
	glMatrixMode(GL_MODELVIEW);

	GetIns().m_windowW = w;
	GetIns().m_windowH = h;
}

//static
void DsEzWindow::_Drag(int x, int y)
{
	//y‚Í‰º•ûŒü‚ª+‚È‚Ì‚Å”½“]
	GetIns().m_pCam->SetMoveDir(x, -y);
}