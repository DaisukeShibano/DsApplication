#include "DsAppPch.h"
#ifndef _DS_RES_PARAM_DS_RAGDOLL_PARAM_
#include "Res/Param/DsRagdollParam.h"
#endif

using namespace std;
using namespace DsPhysics;
using namespace DsApp;

namespace
{
	static DS_RAGDOLL_PARAN_FORMAT s_param[] =
	{
		//mass		//animType		//damperV		//damperA		//ColGroup
		{ 1.0,		0,				10.0,			2.0,			0 },//キーフレーム肌
		{ 1.0,		1,				10.0,			2.0,			0 },//物理肌
		{ 1.0,		0,				10.0,			2.0,			1 },//キーフレーム布
		{ 1.0,		1,				10.0,			2.0,			1 },//物理布
		{ 1.0,		1,				0.1,			0.1,			0 },
	};
}

DsRagdollParam::DsRagdollParam(int id)
	:m_pParam(NULL)
{
	const int maxSize = DS_ARRAY_LENGTH(s_param);
	if (id < maxSize) {
		m_pParam = &s_param[id];
	}
}