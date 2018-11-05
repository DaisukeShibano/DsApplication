#include "DsAppPch.h"
#include "World/Component/Damage/DsDamageComponent.h"
//ëºÇÃÉwÉbÉ_
#include "World/Field/DsFieldChr.h"
#include "Res/Param/DsDamageParam.h"
#include "World/Physics/DsAppCollisionFilter.h"


using namespace DsApp;
using namespace DsPhysics;

DsDamageComponent::DsDamageComponent()
	: m_isRequest(false)
	, m_isCreateDamage(false)
	, m_damageId(-1)
	, m_dmypolyId0(-1)
	, m_dmypolyId1(-1)
	, m_preEndPos(DsVec3d::Zero())
{
}

DsDamageComponent::~DsDamageComponent()
{
}

//virtual
bool DsDamageComponent::Update(const COMPONENT_UPDATE_ARG& arg)
{
	const bool ret = m_isRequest;

	bool isCreateDamage = false;
	if (m_isRequest) {
		DsDamageParam param(m_damageId);
		if (param.IsValid()) {
			DsMat44d start = DsMat44d::Identity();
			const bool isGetDmypoly0 = arg.owner.GetDmypoly(m_dmypolyId0, start, DMYPOLY_SLOT::WEP);
			DsMat44d end = DsMat44d::Identity();
			const bool isGetDmypoly1 = arg.owner.GetDmypoly(m_dmypolyId1, end, DMYPOLY_SLOT::WEP);
			if (isGetDmypoly0 && isGetDmypoly1) {

				const double radius = param.GetRadius();
				DsDbgSys::GetIns().RefDrawCom().DrawCapsule(start.GetPos(), end.GetPos(), 0.1);
				const DsCollisionFilter filter = DsAppCollisionFilter::CalcFilterGroupInsideNoHit(COLLISION_GROUP::DAMAGE);
				DsVec3d hitPos0;
				const bool isHit0 = arg.physWorld.SphereCast(start.GetPos(), end.GetPos(), radius, filter, &arg.owner, &hitPos0);

				//ÇQâÒñ⁄à»ç~ÇÕëOÉtÉåÇ∆ÇÃï‚äÆîªíË
				if (m_isCreateDamage) {
					DsVec3d hitPos1;
					const bool isHit1 = arg.physWorld.SphereCast(m_preEndPos, end.GetPos(), radius, filter, &arg.owner, &hitPos1);
					DsDbgSys::GetIns().RefDrawCom().DrawCapsule(m_preEndPos, end.GetPos(), 0.1);
				}

				isCreateDamage = true;
				m_preEndPos = end.GetPos();
			}
		}
	}

	m_isCreateDamage = isCreateDamage;
	m_isRequest = false;
	return ret;
}

void DsDamageComponent::Request(int damageId, int dmypolyId0, int dmypolyId1)
{
	m_isRequest = true;
	m_damageId = damageId;
	m_dmypolyId0 = dmypolyId0;
	m_dmypolyId1 = dmypolyId1;
}