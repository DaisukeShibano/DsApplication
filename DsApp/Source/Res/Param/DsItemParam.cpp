#include "DsAppPch.h"
#ifndef _DS_ITEM_PARAM_
#include "Res/Param/DsItemParam.h"
#endif

using namespace DsApp;

namespace
{
	static DS_ITEM_PARAM_FORMAT s_param[] =
	{
		//dispName,		//resName
		"",				"",
	};
}

DsItemParam::DsItemParam(int id)
	:m_pParam(NULL)
{
	const int maxSize = DS_ARRAY_LENGTH(s_param);
	if (id < maxSize) {
		m_pParam = &s_param[id];
	}
}
