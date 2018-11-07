#pragma once

#ifndef _DS_COMPONENT_
#include "World/Component/DsComponent.h"
#endif

namespace DsApp
{
	class DsDamageComponent : public DsComponent
	{
	public:
		DsDamageComponent();
		virtual ~DsDamageComponent();

	public:
		virtual bool Update(const COMPONENT_UPDATE_ARG& arg) override;

	public:
		void Request(int damageId, int dmypolyId0, int dmypolyId1);

	private:
		bool m_isRequest;
		bool m_isCreateDamage;
		int m_damageId;
		int m_dmypolyId0;
		int m_dmypolyId1;
		DsVec3d m_preEndPos;
		std::set<const DsFieldObj*> m_hitOwners;

	};
}
