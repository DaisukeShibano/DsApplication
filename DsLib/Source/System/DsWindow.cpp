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
	DS_ASSERT(args.pCam, "ウィンドウ初期化引数が不正");
	DS_ASSERT(args.pLoop, "ウィンドウ初期化引数が不正");
	DS_ASSERT(args.pSys, "ウィンドウ初期化引数が不正");

	m_pCam = args.pCam;
	m_pLoop = args.pLoop;
	m_pSys = args.pSys;
	m_pMouse = new DsMouse();
	DS_ASSERT(m_pMouse, "メモリ確保失敗");
	
	m_pKeyboard = new DsKeyboard();
	DS_ASSERT(m_pKeyboard, "メモリ確保失敗");
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

//ウィンドウの座標を(-1, 1)に収める
DsVec3d DsWindow::GetHalfNormalizeScreenCoord(const DsVec2i& pos) const
{
	const double currX = static_cast<double>(GetCurrentWindowW())*0.5;
	const double currY = static_cast<double>(GetCurrentWindowH())*0.5;
	
	//0, W の範囲を -1, 1 の範囲に収める
	//ウィンドウ左上が(0,0)で右に行くほどxが+で、下に行くほどyが+

	//
	const double x = (currX - static_cast<double>(pos.x)) / (currX);
	const double y = (currY - static_cast<double>(pos.y)) / (currY);

	return DsVec3d(x, y, 0.0);
}

//入力座標をスクリーンサイズで正規化
DsVec3d DsWindow::GetNormalizeScreenCoord(const DsVec2i& pos) const
{
	const double currX = static_cast<double>(GetCurrentWindowW())*0.5;
	const double currY = static_cast<double>(GetCurrentWindowH())*0.5;

	const double x = static_cast<double>(pos.x) / (currX);
	const double y = static_cast<double>(pos.y) / (currY);

	return DsVec3d(x, y, 0.0);
}