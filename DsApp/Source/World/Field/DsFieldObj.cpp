#include "DsAppPch.h"
#ifndef _DS_FIELD_OBJ_H_
#include "World/Field/DsFieldObj.h"
#endif
#ifndef _DS_ANIM_RES_
#include "Res/DsAnimRes.h"
#endif
#ifndef _DS_HITRES_H_
#include "Res/DsHitRes.h"
#endif
#ifndef _DS_MAP_RES_H_
#include "Res/DsMapRes.h"
#endif
#ifndef _DS_ANIM_MODEL_
#include "Animation/DsAnimModel.h"
#endif

using namespace DsLib;
using namespace DsPhysics;
using namespace DsApp;



DsFieldObj::DsFieldObj(DsSys& sys, DsPhysicsWorld& world)
	: m_sys(sys)
	, m_name()
	, m_pAnimation(NULL)
	, m_actorId()
	, m_world(world)
	, m_reqestIsInit(false)
	, m_isCompleteInit(false)
{
	
}

DsFieldObj::~DsFieldObj()
{
	m_actorId = m_world.DeleteActor(m_actorId);
	delete m_pAnimation; m_pAnimation = NULL;
}

//virtual
void DsFieldObj::Initialize(const DsFieldInitInfo& initInfo)
{
	m_name = initInfo.name;

	if (initInfo.pAnimRes)
	{
		m_pAnimation = new DsAnimation(RefAnimController(), *initInfo.pAnimRes, m_sys.RefRender().RefDrawCom());
	}

	if (initInfo.pHitRes)
	{
		if (initInfo.pHitRes->GetAnimRes())
		{
			//�q�b�g���f�����������̂ł�������쐬
			const DsAnimModel* pHitModel = initInfo.pHitRes->GetAnimRes()->CreateAnimModel();
			if (pHitModel)
			{
				DsRigidMesh::DsRigidMeshFactory factory(*pHitModel, m_name.c_str());
				//�����ʒu�{SetPosition�������Ă��܂��̂ŁA�����ł͐ݒ肹���A����SetPosition���Ă�
				//factory.InitPos(initInfo.pos);
				//factory.InitRot(initInfo.rot);
				DsActor::Option option = DsActor::Option::Default();
				switch (initInfo.physicsType)
				{
				case DS_MAP_OBJ_TYPE::STATIC:
					option = DsActor::Option::Static();
					break;
				case DS_MAP_OBJ_TYPE::DYNAMIC:
					option = DsActor::Option::Default();
					break;
				case DS_MAP_OBJ_TYPE::AUTONOMOUS:
					option = DsActor::Option::Default();
					break;
				case DS_MAP_OBJ_TYPE::CONTROL:
					option = DsActor::Option::ChrProxy();
					break;
				default:
					break;
				}
				factory.SetOption(option);

				m_actorId = m_world.CreateActor(factory);
				SetPosition(initInfo.pos);
				SetRotation(initInfo.rot);
				DsActor* pActor = m_world.GetActor(m_actorId);
				if (pActor)
				{
					pActor->SetMaterial(DsActorMaterial::Aluminum());
					if (m_pAnimation)
					{
						m_pAnimation->SetRootMatrix(GetPosition(), GetRotation());
					}
				}
				delete pHitModel;//m_world.CreateActor������͂��������͂�
			}
			else
			{
				DS_LOG("�I�u�W�F�쐬���A�j�����f�������������̂ŃI�u�W�F�͍���܂���ł��� %s", m_name.c_str());
			}
		}
		else
		{
			//�q�b�g���f�����Ȃ��̂Ō`���񂩂�쐬
			DsVec3d vertex[8];
			const DsHitRes::Shape& shape = initInfo.pHitRes->RefSpahe();
			DsRigidBox::GetVertex(vertex, shape.sizeX, shape.sizeY, shape.sizeZ);
			{
				DsRigidBox::DsRigidBoxFactory factory(vertex, shape.weight, m_name.c_str());
				//�����ʒu�{SetPosition�������Ă��܂��̂ŁA�����ł͐ݒ肹���A����SetPosition���Ă�
				//factory.InitPos(initInfo.pos);
				//factory.InitRot(initInfo.rot);
				DsRigidBox::Option option = DsRigidBox::Option::Default();
				switch (initInfo.physicsType)
				{
				case DS_MAP_OBJ_TYPE::STATIC:
					option = DsActor::Option::Static();
					break;
				case DS_MAP_OBJ_TYPE::DYNAMIC:
					option = DsActor::Option::Default();
					break;
				case DS_MAP_OBJ_TYPE::AUTONOMOUS:
					option = DsActor::Option::Default();
					break;
				case DS_MAP_OBJ_TYPE::CONTROL:
					option = DsActor::Option::ChrProxy();
					break;
				default:
					break;
				}
				factory.SetOption(option);
				
				m_actorId = m_world.CreateActor(factory);
				SetPosition(initInfo.pos);
				SetRotation(initInfo.rot);
				DsActor* pActor = m_world.GetActor(m_actorId);
				if (pActor)
				{
					pActor->SetMaterial(DsActorMaterial::Aluminum());
					if (m_pAnimation)
					{
						m_pAnimation->SetRootMatrix(GetPosition(), GetRotation());
					}
				}
			}
		}
	}
	else
	{
		DS_LOG("�I�u�W�F��������񂪖��������̂ŃI�u�W�F�͍���܂���ł��� %s", m_name.c_str());
	}

	if (m_pAnimation)
	{
		m_pAnimation->RegisterDraw();
	}

	m_reqestIsInit = true;
	m_isCompleteInit = true;
}

//virtual 
void DsFieldObj::Update(double dt)
{
	if (m_pAnimation)
	{
		m_pAnimation->Update(dt);
		const DsActor* pActor = m_world.GetActor(m_actorId);
		if (pActor)
		{
			m_pAnimation->SetRootMatrix(GetPosition(), pActor->GetRotation());
		}
	}
}

//virtual
void DsFieldObj::SetPosition(const DsVec3d& pos)
{
	DsActor* pActor = m_world.GetActor(m_actorId);
	if (pActor)
	{
		const DsVec3d offset = DsVec3d(0, pActor->RefAabb().GetMax().y, 0);
		pActor->SetPosition(pos + offset);
	}

	if (m_pAnimation)
	{
		m_pAnimation->SetRootMatrix(GetPosition(), GetRotation());
	}
}
//virtual
void DsFieldObj::SetRotation(const DsMat33d& rot)
{
	DsActor* pActor = m_world.GetActor(m_actorId);
	if (pActor)
	{
		pActor->SetRotation(rot);
	}

	if (m_pAnimation)
	{
		m_pAnimation->SetRootMatrix(GetPosition(), GetRotation());
	}
}

//virtual
DsVec3d DsFieldObj::GetPosition() const
{
	DsActor* pActor = m_world.GetActor(m_actorId);
	if (pActor)
	{
		const DsVec3d offset = DsVec3d(0, -pActor->RefAabb().GetMax().y, 0);
		return pActor->GetPosition() + offset;
	}
	else
	{
		return DsVec3d::Zero();
	}
}

//virtual
DsMat33d DsFieldObj::GetRotation() const
{
	DsActor* pActor = m_world.GetActor(m_actorId);
	if (pActor)
	{
		return pActor->GetRotation();
	}
	else
	{
		return DsMat33d::Identity();
	}
}

DsActor* DsFieldObj::GetActor()
{
	return m_world.GetActor(m_actorId);
}

const DsActor* DsFieldObj::GetActor() const
{
	return m_world.GetActor(m_actorId);
}

//virtual 
DsAnimController& DsFieldObj::RefAnimController()
{
	return DsAnimController::Null();
}


//virtual 
void DsFieldObj::DbgDraw(DsLib::DsDrawCommand& com)
{
	DsActor* pActor = m_world.GetActor(m_actorId);
	if (pActor)
	{
		//���b�V���͕`�惂�f���Ɣ��̂Œe��
		if (pActor->GetType() != DsActor::RIGID_MESH)
		{
			pActor->SetColor(DsVec4d(1, 1, 1, 0));
			pActor->SetLineColor(DsVec4d(0, 0, 0, 0));
			pActor->Draw(com);
		}
	}
}