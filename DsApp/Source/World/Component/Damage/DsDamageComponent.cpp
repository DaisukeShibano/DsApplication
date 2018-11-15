#include "DsAppPch.h"
#include "World/Component/Damage/DsDamageComponent.h"
//���̃w�b�_
#include "World/Field/DsFieldChr.h"
#include "Res/Param/DsDamageParam.h"
#include "World/Physics/DsAppCollisionFilter.h"
#include "World/Component/DsComponentSystem.h"


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

				//�Q��ڈȍ~�͑O�t���Ƃ̕⊮����
				if (!isHit) {
					if (m_isCreateDamage) {
						isHit = arg.physWorld.SphereCast(m_preEndPos, end.GetPos(), radius, filter, &arg.owner, results);
						//DsDbgSys::GetIns().RefDrawCom().DrawCapsule(m_preEndPos, end.GetPos(), radius);
					}
				}


				//���i�q�b�g��𐧌�
				if (isHit) {
					for (const DsCollisionResult& result : results) {
						const DsFieldObj* pOwner1 = (DsFieldObj*)(result.GetOwnerId1()->GetActor()->GetUserData());
						const DsFieldObj* pOwner2 = (DsFieldObj*)(result.GetOwnerId2()->GetActor()->GetUserData());
						const DsFieldObj* pDefender = (pOwner1 && (pOwner1 != &arg.owner)) ? (pOwner1) : (pOwner2);
						if (pDefender) {
							if (m_hitOwners.find(pDefender) == m_hitOwners.end()) {
								//����q�b�g

								//�ގ��ŃG�t�F�N�g�ς���Ƃ����肻���Ȃ̂ő��葤�Ƀ��N�G�X�g
								DsComponentSystem* pDefCom = pDefender->GetComponentSystem();
								if (pDefCom) {
									const DsVec3d hitPos = result.GetPos()[0];
									const DsVec3d hitDir = DsVec3d::Normalize(end.GetPos() - m_preEndPos);//�����̍����𑬓x�Ƃ��ēn���Ă���������
									pDefCom->RequestHitEffect(m_hitOwners.size(), param.GetHitEffectId(), hitPos, hitDir);
								}

								//�q�b�g�o�^
								m_hitOwners.insert(pDefender);
							}
							else {
								//�Q��ڈȍ~�̃q�b�g
								//�������Ȃ�
							}
						}
					}
				}


				//HP���Z
				//�m�b�N�o�b�N���A�^�b�`
				

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