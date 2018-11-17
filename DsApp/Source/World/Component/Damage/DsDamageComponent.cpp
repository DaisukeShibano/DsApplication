#include "DsAppPch.h"
#include "World/Component/Damage/DsDamageComponent.h"
//他のヘッダ
#include "World/Field/DsFieldChr.h"
#include "Res/Param/DsDamageParam.h"
#include "World/Physics/DsAppCollisionFilter.h"
#include "World/Component/DsComponentSystem.h"
#include "World/Field/Action/DsActionFlags.h"


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
				//DsDbgSys::GetIns().RefDrawCom().DrawCapsule(start.GetPos(), end.GetPos(), radius);
				const DsCollisionFilter filter = DsAppCollisionFilter::CalcFilterGroupInsideNoHit(COLLISION_GROUP::DAMAGE);
				std::vector<DsCollisionResult> results;
				bool isHit = arg.physWorld.SphereCast(start.GetPos(), end.GetPos(), radius, filter, &arg.owner, results);

				//２回目以降は前フレとの補完判定
				if (!isHit) {
					if (m_isCreateDamage) {
						isHit = arg.physWorld.SphereCast(m_preEndPos, end.GetPos(), radius, filter, &arg.owner, results);
						//DsDbgSys::GetIns().RefDrawCom().DrawCapsule(m_preEndPos, end.GetPos(), radius);
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

								//ヒットエフェクト
								DsComponentSystem* pDefCom = pDefender->GetComponentSystem();
								if (pDefCom) {
									const DsVec3d hitPos = result.GetPos()[0];
									const DsVec3d hitDir = DsVec3d::Normalize(end.GetPos() - m_preEndPos);//ここの差分を速度として渡してもいいかも
									pDefCom->RequestHitEffect(m_hitOwners.size(), param.GetHitEffectId(), hitPos, hitDir);
								}

								const DsVec3d atk = arg.owner.GetPosition();
								const DsVec3d def = pDefender->GetPosition();
								const DsVec3d toAtk = DsVec3d::Normalize(DsVec3d(atk.x, 0, atk.z)- DsVec3d(def.x, 0,def.z) );
								const DsVec3d defDir= pDefender->GetRotation().GetAxisZ();
								const double sign = (0.0 < DsVec3d::Cross(defDir, toAtk).y) ? (1.0) :(-1.0);
								const double ang = DsACos(DsVec3d::Dot(defDir, toAtk));

								//ダメージリアクション
								DsActionFlags* pFlags = pDefender->GetActionFlags();
								if (pFlags) {

									DsDmgDir dmgDir = DsDmgDir::F;
									if (ang < M_PI*0.25) {
										dmgDir = DsDmgDir::F;
									}
									else if ((M_PI*0.25 <= ang) && (ang <= M_PI * 0.75)) {
										if (0.0 < sign) {
											dmgDir = DsDmgDir::L;
										}
										else {
											dmgDir = DsDmgDir::R;
										}
									}
									else {
										dmgDir = DsDmgDir::B;
									}
									pFlags->SetDmgDir(dmgDir);
								}

								//ノックバック
								const double kLenDef = param.GetKnockBackLen()*0.65;
								const double kLenAtk = param.GetKnockBackLen()*0.35;//相手が下がれなかった分下がるのが理想。
								const double kTime = param.GetKnockBackTime();
								DsComponentSystem* pAtkCom = arg.owner.GetComponentSystem();
								if (pDefCom) {//相手が下がる分
									pDefCom->RequestKnockBack(-toAtk * kLenDef, kTime);
								}
								if (pAtkCom) {//自分が下がる分。
									pAtkCom->RequestKnockBack(toAtk * kLenAtk, kTime);
								}

								//ヒット登録
								m_hitOwners.insert(pDefender);
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