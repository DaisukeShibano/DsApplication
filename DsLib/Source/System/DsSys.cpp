#include "DsPch.h"
#ifndef _DS_SYS_H_
#include "System/DsSys.h"
#endif
#include "System/DsWindow.h"
#include "System/DsMainLoop.h"
#include "Graphics/Render/DsRender.h"
#include "Graphics/Camera/DsCamera.h"
#include "Config/DsConf.h"
#include "System/DsDbgSys.h"
#ifndef _DS_RESOURCE_
#include "Res/DsResource.h"
#endif
#ifndef _DS_WINDOW_MANAGER_H_
#include "System/DsWindowManager.h"
#endif

using namespace DsLib;

DsSys::DsSys()
: m_isSetUp(false)
, m_pWindow(NULL)
, m_pRender(NULL)
, m_pCam(NULL)
, m_pRes(NULL)
{
}

DsSys::~DsSys()
{
	delete m_pWindow; m_pWindow = NULL;
	delete m_pRender; m_pRender = NULL;
	delete m_pCam; m_pCam = NULL;
	delete m_pRes; m_pRes = NULL;
	DsDbgSys::Finalize();
}

void DsSys::Setup( DsSysArgs& args )
{
	DsConf::GetIns().Initialize(args.pConfPath);
	m_pWindow = DsWindowManager::Create(static_cast<DS_WINDOW_SYSTEM_TYPE>(args.windowType));
	m_pCam = new DsCamera();
	DS_ASSERT(m_pCam , "DsSys‚Ì‰Šú‰»‚É‚ÉŽ¸”s");
	m_pRes = new DsResource();
	DS_ASSERT(m_pRes, "DsSys‚Ì‰Šú‰»‚É‚ÉŽ¸”s");

	DsWindow::InitArgs winArgs;
	winArgs.argc = args.argc;
	winArgs.argv = args.argv;
	winArgs.pCam = m_pCam;
	winArgs.pLoop = args.pLoop;
	winArgs.pSys = this;
	winArgs.handle = args.windowHandle;
	m_pWindow->Initialize(winArgs);
	m_pRender = &DsRender::Create(*m_pCam, *this);
	DsDbgSys::Initialize(*this);
	

	if (m_pWindow && m_pCam && m_pRender)
	{
		m_isSetUp = true;
	}
	else
	{
		m_isSetUp = false;
	}

	if (m_isSetUp)
	{
		m_pWindow->Setup();
		//«glutŽg—p‚Ìê‡‚±‚±‚©‚çæ‚Í—ˆ‚È‚¢
	}
}

void DsSys::Update()
{
	if (IsSetUp())
	{
		DsPerf::RefPerfTotalFps().End();
		DsPerf::RefPerfTotalFps().Begin();
		const double dt = GetDt();
		m_pCam->Update(dt);
		m_pRender->Update( dt );
		m_pWindow->Update(dt);
		DsPerf::RefPerfDefault().DbgPrintPerf();
		//DsPerf::RefPerfTotalFps().DbgPrintFps();
	}
}

bool DsSys::IsSetUp() const
{
	return m_isSetUp;
}

double DsSys::GetDt() const
{
	return IsSetUp() ? m_pWindow->GetDt() : 0.0;
}

DsMouse& DsSys::RefMouse()
{
	return m_pWindow->RefMouse();
}
const DsMouse& DsSys::RefMouse() const
{
	return m_pWindow->RefMouse();
}

DsKeyboard& DsSys::RefKeyboard()
{
	return m_pWindow->RefKeyboard();
}
const DsKeyboard& DsSys::RefKeyboard() const
{
	return m_pWindow->RefKeyboard();
}