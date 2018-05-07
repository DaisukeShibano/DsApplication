#ifndef _DS_COMPONENT_SYSTEM_
#define _DS_COMPONENT_SYSTEM_

#ifndef _DS_COMPONENT_SYSTEM_DEFINE_
#include "World\/Component/DsComponentSystemDefine.h"
#endif

namespace DsApp
{
	class DsComponent;
}

namespace DsApp
{
	class DsComponentSystem
	{
	public:
		DsComponentSystem();
		virtual ~DsComponentSystem();

	public:
		void Update(const COMPONENT_UPDATE_ARG& arg);

	public:
		void RequestTraceEffect(ds_int64 key, int effectId, int dmypolyId0, int dmypolyId1);

	private:
		std::map<ds_uint64, DsComponent*> m_components;
	};




}


#endif