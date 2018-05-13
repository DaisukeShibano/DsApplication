#ifndef _DS_EQUIP_COMPONENT_
#define _DS_EQUIP_COMPONENT_

#ifndef _DS_COMPONENT_
#include "World/Component/DsComponent.h"
#endif
#ifndef _DS_ATTACH_ENTITY_
#include "World/Component/Attach/DsAttachEntity.h"
#endif

namespace DsApp
{
}

namespace DsApp
{
	class DsAnimAttachEntity : public DsAttachEntity
	{
	public:
		DsAnimAttachEntity():m_pAnim(NULL){}
	public:
		virtual void SetTransform(const DsMat44d& transform) override;
		void SetAnim(DsLib::DsAnimation* pAnim) { m_pAnim = pAnim; }
	private:
		DsLib::DsAnimation* m_pAnim;
	};

	class DsEquipComponent : public DsComponent
	{
	public:
		DsEquipComponent();
		virtual ~DsEquipComponent();

	public:
		virtual bool Update(const COMPONENT_UPDATE_ARG& arg) override;
		DsLib::DsAnimation* GetWep()const { return m_pWep; }

	private:
		int m_wepIndex;
		DsLib::DsAnimation* m_pWep;
		DsAnimAttachEntity m_attachEntity;
		int m_attachDmypolyId;
	};


}


#endif