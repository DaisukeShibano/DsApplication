#include "DsAppPch.h"
#include "World/Component/Damage/DsDamageComponent.h"
//他のヘッダ
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
	, m_hitOwners()
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
				DsDbgSys::GetIns().RefDrawCom().DrawCapsule(start.GetPos(), end.GetPos(), radius);
				const DsCollisionFilter filter = DsAppCollisionFilter::CalcFilterGroupInsideNoHit(COLLISION_GROUP::DAMAGE);
				std::vector<DsCollisionResult> results;
				bool isHit = arg.physWorld.SphereCast(start.GetPos(), end.GetPos(), radius, filter, &arg.owner, results);

				//２回目以降は前フレとの補完判定
				if (!isHit) {
					if (m_isCreateDamage) {
						DsVec3d hitPos1;
						isHit = arg.physWorld.SphereCast(m_preEndPos, end.GetPos(), radius, filter, &arg.owner, results);
						DsDbgSys::GetIns().RefDrawCom().DrawCapsule(m_preEndPos, end.GetPos(), radius);
					}
				}


				//多段ヒット回避制御
				if (isHit) {
					for (const DsCollisionResult& result : results) {
						const DsFieldObj* pOwner1 = (DsFieldObj*)(result.GetOwnerId1()->GetActor()->GetUserData());
						const DsFieldObj* pOwner2 = (DsFieldObj*)(result.GetOwnerId2()->GetActor()->GetUserData());
						const DsFieldObj* pDefender = (pOwner1 && (pOwner1 != &arg.owner)) ? (pOwner1) : (pOwner2);
						if (pDefender) {
							if (m_hitOwners.find(pDefender) == m_hitOwners.end()) {
								//初回ヒット
								m_hitOwners.insert(pDefender);
								//オーナーにダメージ通知
							}
							else {
								//２回目以降のヒット
								//何もしない
							}
						}
					}
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