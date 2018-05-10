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
#ifndef _DS_ANIM_EVENT_CALLBACK_
#include "World/Animation/Event/DsAnimEventCallback.h"
#endif
#ifndef _DS_COMPONENT_SYSTEM_
#include "World/Component/DsComponentSystem.h"
#endif

using namespace DsLib;
using namespace DsPhysics;
using namespace DsApp;



DsFieldObj::DsFieldObj(DsSys& sys, DsPhysicsWorld& world, DsLib::DsResource& resource)
	: m_sys(sys)
	, m_name()
	, m_resource(resource)
	, m_pAnimation(NULL)
	, m_actorId()
	, m_world(world)
	, m_pAnimEventCallback(NULL)
	, m_pComponentSystem(NULL)
	, m_reqestIsInit(false)
	, m_isCompleteInit(false)
	, m_isRequestDirectAnim(false)
{
	
}

DsFieldObj::~DsFieldObj()
{
	m_actorId = m_world.DeleteActor(m_actorId);
	delete m_pAnimEventCallback; m_pAnimEventCallback = NULL;
	delete m_pAnimation; m_pAnimation = NULL;
	delete m_pComponentSystem; m_pComponentSystem = NULL;
}

namespace
{
	DsActor::Option _GetActorOption(const DS_MAP_OBJ_TYPE type)
	{
		DsActor::Option option = DsActor::Option::Default();
		switch (type)
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
		return option;
	}
}

//virtual
void DsFieldObj::Initialize(const DsFieldInitInfo& initInfo)
{
	m_name = initInfo.name;

	m_pComponentSystem = new DsComponentSystem();
	DS_ASSERT(m_pComponentSystem, "メモリ確保失敗");

	if (initInfo.pAnimRes)
	{
		m_pAnimation = new DsAnimation(*initInfo.pAnimRes, m_sys.RefRender().RefDrawCom());
		DS_ASSERT(m_pAnimation, "メモリ確保失敗");
		m_pAnimEventCallback = new DsAnimEventCallback(*this, m_resource);
		DS_ASSERT(m_pAnimEventCallback, "メモリ確保失敗");
	}

	if (initInfo.pHitRes)
	{
		if (initInfo.pHitRes->GetAnimRes())
		{
			//ヒットモデルがあったのでそこから作成
			const DsAnimModel* pHitModel = initInfo.pHitRes->GetAnimRes()->CreateAnimModel();
			if (pHitModel)
			{
				DsRigidMesh::DsRigidMeshFactory factory(*pHitModel, m_name.c_str());
				_SetActorCoord(factory, initInfo);
				const DsActor::Option option = _GetActorOption(initInfo.physicsType);
				factory.SetOption(option);
				m_actorId = m_world.CreateActor(factory);
				DsActor* pActor = m_world.GetActor(m_actorId);
				if (pActor)
				{
					pActor->SetMaterial(DsActorMaterial::Aluminum());
					if (m_pAnimation)
					{
						m_pAnimation->SetRootMatrix(GetPosition(), GetRotation());
					}
				}
				delete pHitModel;//m_world.CreateActorした後はもういらんはず
			}
			else
			{
				DS_LOG("オブジェ作成中アニメモデルが無かったのでオブジェは作られませんでした %s", m_name.c_str());
			}
		}
		else
		{
			//ヒットモデルがないので形状情報から作成
			DsVec3d vertex[8];
			const DsHitRes::Shape& shape = initInfo.pHitRes->RefSpahe();
			DsRigidBox::GetVertex(vertex, shape.sizeX, shape.sizeY, shape.sizeZ);
			{
				DsRigidBox::DsRigidBoxFactory factory(vertex, shape.weight, m_name.c_str());
				_SetActorCoord(factory, initInfo);
				const DsActor::Option option = _GetActorOption(initInfo.physicsType);
				factory.SetOption(option);
				
				m_actorId = m_world.CreateActor(factory);
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
		DS_LOG("オブジェ初期化情報が無かったのでオブジェは作られませんでした %s", m_name.c_str());
	}

	if (m_pAnimation)
	{
		m_pAnimation->RegisterDraw();
	}

	m_reqestIsInit = true;
	m_isCompleteInit = true;
}

//virtual
void DsFieldObj::_SetActorCoord(DsPhysics::DsActorCoordFactory& factory, const DsFieldInitInfo& initInfo)
{
	const DsMat33d rot = DsMat33d::RotateX(initInfo.ang.x)*DsMat33d::RotateY(initInfo.ang.y)*DsMat33d::RotateZ(initInfo.ang.z);
	factory.InitPos(initInfo.pos);
	factory.InitRot(rot);
}

//virtual 
void DsFieldObj::Update(double dt)
{
	if (m_pAnimation)
	{
		if (m_isRequestDirectAnim) {
			dt = 0;
		}
		m_pAnimation->Update(dt);
		m_isRequestDirectAnim = false;
		const DsActor* pActor = m_world.GetActor(m_actorId);
		m_pAnimation->SetRootMatrix(GetPosition(), GetRotation());
	}

	if (m_pAnimEventCallback) {
		m_pAnimEventCallback->Call();
	}

	if (m_pComponentSystem) {
		COMPONENT_UPDATE_ARG arg(dt, *this, m_sys, _GetActionRequest());
		m_pComponentSystem->Update(arg);
	}
}

//virtual
void DsFieldObj::SetPosition(const DsVec3d& pos)
{
	DsActor* pActor = m_world.GetActor(m_actorId);
	if (pActor)
	{
		const double y = pActor->GetPosition().y;
		const DsVec3d offset = DsVec3d(0, pActor->RefAabb().GetMax().y-y, 0);
		pActor->SetPosition(pos);

		if (m_pAnimation)
		{
			m_pAnimation->SetRootMatrix(GetPosition(), GetRotation());
		}
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
		const double y = pActor->GetPosition().y;
		const DsVec3d offset = DsVec3d(0, -pActor->RefAabb().GetMax().y-y, 0);
		return pActor->GetPosition();
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

DsMat44d DsFieldObj::GetDmypoly(int id)const
{
	DsMat44d ret = DsMat44d::Identity();
	const DsAnimation* pAnim = GetAnim();
	if (pAnim) {
		const DsAnimModel* pModel = pAnim->GetModel();
		if (pModel) {
			ret = pModel->GetDmypoly(id);
		}
	}
	return ret;
}

//virtual
void DsFieldObj::SetRequestAnim(std::string name)
{
	if (m_pAnimation) {
		for (DsAnimClip* pClip : m_pAnimation->RefAnimClips()) {
			if (name == pClip->RefAnimName()) {
				m_pAnimation->RequestPlayAnim(pClip);
				m_isRequestDirectAnim = true;
				break;
			}
		}
	}
}


//virtual 
void DsFieldObj::DbgDraw(DsLib::DsDrawCommand& com)
{
	DsActor* pActor = m_world.GetActor(m_actorId);
	if (pActor)
	{
		//メッシュは描画モデルと被るので弾く
		if (pActor->GetType() != DsActor::RIGID_MESH)
		{
			pActor->SetColor(DsVec4d(1, 1, 1, 0));
			pActor->SetLineColor(DsVec4d(0, 0, 0, 0));
			pActor->Draw(com);
		}
	}
}