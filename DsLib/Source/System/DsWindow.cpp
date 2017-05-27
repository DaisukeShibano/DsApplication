#include "DsPch.h"

#ifndef _DS_WINDOW_H_
#include "System/DsWindow.h"
#endif

#include "Graphics/Camera/DsCamera.h"
#include "Graphics/Render/DsRender.h"
#include "System/DsMainLoop.h"
#include "System/DsSys.h"
#include "Mouse/DsMouse.h"
#ifndef _DS_KEYBOARD_H_
#include "Keyboard/DsKeyboard.h"
#endif

using namespace DsLib;

DsWindow::DsWindow()
	: m_windowW(INIT_WINDOW_W)
	, m_windowH(INIT_WINDOW_H)
	, m_mspf(1.0f / (float)(DEFAULT_FPS))
	, m_pCam(NULL)
	, m_pLoop(NULL)
	, m_pSys(NULL)
	, m_pMouse(NULL)
	, m_pKeyboard(NULL)
{

}

DsWindow::~DsWindow()
{
	delete m_pMouse; m_pMouse = NULL;
	delete m_pKeyboard; m_pKeyboard = NULL;
}

void DsWindow::Update(double dt)
{
	m_pMouse->Update(dt);
}

//virtual 
void DsWindow::Initialize(InitArgs& args)
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
	m_pMouse = new DsMouse();
	if (NULL == m_pMouse)
	{
		DsError::Panic("�������m�ێ��s\n");
	}
	m_pKeyboard = new DsKeyboard();
	if(NULL == m_pKeyboard)
	{
		DsError::Panic("�������m�ێ��s\n");
	}
}

//virtual 
void DsWindow::Setup()
{
	DsMainLoopArgs args(
		m_pSys->RefRender().RefDrawCom(),
		m_pSys->RefRender().GetRenderTool(),
		*m_pSys,
		m_pSys->GetDt());
	m_pLoop->Initialize(args);
}

//�E�B���h�E�̍��W��(-1, 1)�Ɏ��߂�
DsVec3d DsWindow::GetHalfNormalizeScreenCoord(const DsVec2i& pos) const
{
	const double currX = static_cast<double>(GetCurrentWindowW())*0.5;
	const double currY = static_cast<double>(GetCurrentWindowH())*0.5;
	
	//0, W �͈̔͂� -1, 1 �͈̔͂Ɏ��߂�
	//�E�B���h�E���オ(0,0)�ŉE�ɍs���ق�x��+�ŁA���ɍs���ق�y��+

	//
	const double x = (currX - static_cast<double>(pos.x)) / (currX);
	const double y = (currY - static_cast<double>(pos.y)) / (currY);

	return DsVec3d(x, y, 0.0);
}

//���͍��W���X�N���[���T�C�Y�Ő��K��
DsVec3d DsWindow::GetNormalizeScreenCoord(const DsVec2i& pos) const
{
	const double currX = static_cast<double>(GetCurrentWindowW())*0.5;
	const double currY = static_cast<double>(GetCurrentWindowH())*0.5;

	const double x = static_cast<double>(pos.x) / (currX);
	const double y = static_cast<double>(pos.y) / (currY);

	return DsVec3d(x, y, 0.0);
}