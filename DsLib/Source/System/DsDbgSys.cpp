#include "DsPch.h"
#ifndef _DS_DBG_SYS_H_
#include "System/DsDbgSys.h"
#endif
#include "System/DsSys.h"
#include "Graphics/Render/DsRender.h"

using namespace DsLib;

static DsDbgSys* s_pDbgSys = NULL;

DsDbgSys::DsDbgSys( DsSys& sys )
:m_sys(sys)
{
}

DsDbgSys::~DsDbgSys()
{
}

//static
void DsDbgSys::Initialize( DsSys& sys )
{
	s_pDbgSys = new DsDbgSys(sys);
	DS_ASSERT(s_pDbgSys, "ÉÅÉÇÉäämï€é∏îs");
}

//static
void DsDbgSys::Finalize()
{
	delete s_pDbgSys; s_pDbgSys = NULL;
}

//static
DsDbgSys& DsDbgSys::GetIns()
{
	return *s_pDbgSys;
}

DsDrawCommand& DsDbgSys::RefDrawCom()
{
	return m_sys.RefRender().RefDrawCom();
}

DsMouse& DsDbgSys::RefMouse()
{
	return m_sys.RefMouse();
}

DsCamera& DsDbgSys::RefCam()
{
	return m_sys.RefCam();
}

DsWindow& DsDbgSys::RefWindow()
{
	return m_sys.RefWindow();
}