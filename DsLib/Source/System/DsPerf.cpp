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