#ifndef _DS_EQUIP_COMPONENT_
#define _DS_EQUIP_COMPONENT_

#ifndef _DS_COMPONENT_
#include "World/Component/DsComponent.h"
#endif

namespace DsApp
{
	class DsEquipComponent : public DsComponent
	{
	public:
		DsEquipComponent();
		virtual ~DsEquipComponent();

	public:
		virtual bool Update(const COMPONENT_UPDATE_ARG& arg) override;

	private:
		int m_wepIndex;

	};


}


#endif