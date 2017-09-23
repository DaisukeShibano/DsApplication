#include "DsPch.h"
#ifndef _DS_PERF_H_
#include "System/DsPerf.h"
#endif


using namespace DsLib;

DsPerf& DsPerf::RefPerfDefault()
{
	static DsPerf ret;
	return ret;
}

DsPerf& DsPerf::RefPerfTotalFps()
{
	static DsPerf ret;
	return ret;
}

void DsPerf::DbgDrawFps()
{
	DsDbgSys::GetIns().RefDrawCom().SetColor(DsVec3d(0,0,0)).DrawTextScreen(DsVec2d(0.3, 0.3), L"FPS:%.1f", m_fps );
}