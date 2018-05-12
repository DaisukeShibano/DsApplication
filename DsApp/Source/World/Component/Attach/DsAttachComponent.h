#ifndef _DS_ATTACH_COMPONENT_
#define _DS_ATTACH_COMPENENT_

#ifndef _DS_COMPONENT_
#include "World/Component/DsComponent.h"
#endif

namespace DsApp
{
	class DsFieldObj;
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
		void Request(const DsMat44d target, DsFieldObj* pMove);

	private:
		DsMat44d m_target;
		DsFieldObj* m_pMove;
		bool m_reqAttach;

	};


}

#endif
