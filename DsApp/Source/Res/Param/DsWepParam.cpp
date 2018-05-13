#include "DsAppPch.h"
#ifndef _DS_WEP_PARAM_
#include "Res/Param/DsWepParam.h"
#endif

using namespace DsApp;

namespace
{
	static DS_WEP_PARAM_FORMAT s_param[] =
	{
		//dispName,		//resName				//attachDmypolyId		//damageId
		"ƒiƒCƒt",		"aliceBlade.dmdl",		0,						0,
	};
}

DsWepParam::DsWepParam(int id)
	:m_pParam(NULL)
{
	const int maxSize = DS_ARRAY_LENGTH(s_param);
	if (id < maxSize) {
		m_pParam = &s_param[id];
	}
}
