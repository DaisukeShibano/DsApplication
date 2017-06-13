#include "DsPhysicsPch.h"
#ifndef _DS_RES_PARAM_DS_RAGDOLL_PARAM_
#include "Res/Param/DsRagdollParam.h"
#endif

using namespace std;
using namespace DsPhysics;


static DsRagdollParamST s_param[]=
{
	{1.0},
};


DsRagdollParam::DsRagdollParam(int id)
	:m_pParam(NULL)
{
	const int maxSize = sizeof(s_param) / sizeof(s_param[0]);
	if (id < maxSize) {
		m_pParam = &s_param[id];
	}
}