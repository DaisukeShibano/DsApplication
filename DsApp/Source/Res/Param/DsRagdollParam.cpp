#include "DsAppPch.h"
#ifndef _DS_RES_PARAM_DS_RAGDOLL_PARAM_
#include "Res/Param/DsRagdollParam.h"
#endif

using namespace std;
using namespace DsPhysics;
using namespace DsApp;

static DsRagdollParamST s_param[]=
{
	//mass		//animType		//damperV		//damperA		//ColGroup
	{ 1.0,		0,				10.0,			2.0,			0 },
	{ 1.0,		1,				10.0,			2.0,			0 },
	{ 1.0,		0,				10.0,			2.0,			1 },
	{ 1.0,		1,				10.0,			2.0,			1 },
};


DsRagdollParam::DsRagdollParam(int id)
	:m_pParam(NULL)
{
	const int maxSize = sizeof(s_param) / sizeof(s_param[0]);
	if (id < maxSize) {
		m_pParam = &s_param[id];
	}
}