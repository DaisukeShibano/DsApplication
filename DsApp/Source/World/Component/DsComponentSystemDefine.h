#ifndef _DS_COMPONENT_SYSTEM_DEFINE_
#define _DS_COMPONENT_SYSTEM_DEFINE_

/*
#ifndef _DS_COMPONENT_SYSTEM_DEFINE_
#include "World\/Component/DsComponentSystemDefine.h"
#endif
*/
namespace DsApp
{
	class DsFieldObj;
	class DsActionRequest;
}

namespace DsApp
{
	struct COMPONENT_UPDATE_ARG
	{
		COMPONENT_UPDATE_ARG(double _dt, DsFieldObj& _owner, DsLib::DsSys& _sys, const DsActionRequest* pActReq)
			: dt(_dt)
			, owner(_owner)
			, sys(_sys)
			, m_pActReq(pActReq)
		{}
		double dt;
		DsFieldObj& owner;
		DsLib::DsSys& sys;
		const DsActionRequest* m_pActReq;
	};

}


#endif
