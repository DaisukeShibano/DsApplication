#ifndef _DS_ATTACH_COMPONENT_
#define _DS_ATTACH_COMPENENT_

#ifndef _DS_COMPONENT_
#include "World/Component/DsComponent.h"
#endif

namespace DsApp
{
	class DsFieldObj;
	class DsAttachEntity;
}

namespace DsApp
{
	class DsAttachComponent : public DsComponent
	{
	public:
		DsAttachComponent();
		virtual ~DsAttachComponent();

	public:
		virtual bool Update(const COMPONENT_UPDATE_ARG& arg) override;
		void Request(const DsMat44d target, DsAttachEntity* pMove);

	private:
		DsMat44d m_target;
		DsAttachEntity* m_pMove;
		bool m_reqAttach;

	};


}

#endif
