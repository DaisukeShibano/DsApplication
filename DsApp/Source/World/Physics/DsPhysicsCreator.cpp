#include "DsAppPch.h"
//�����̃w�b�_
#include "World/Physics/DsPhysicsCreator.h"
//���̑��̃w�b�_
#include "Res/DsMapRes.h"

using namespace DsLib;
using namespace DsPhysics;
using namespace DsApp;


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


	void _SetActorCoord(DsPhysics::DsActorCoordFactory& factory, const DsVec3d& pos, const DsVec3d& ang)
	{
		const DsMat33d rot = DsMat33d::RotateX(ang.x)*DsMat33d::RotateY(ang.y)*DsMat33d::RotateZ(ang.z);
		factory.InitPos(pos);
		factory.InitRot(rot);
	}
}


DsActorId DsPhysicsCreator::Create(DsPhysicsWorld& world, const DsHitRes& hitRes, DS_MAP_OBJ_TYPE type, const DsVec3d& pos, const DsVec3d& ang, const char* pInsName)
{
	DsActorId ret;

	if (hitRes.GetAnimRes())
	{
		//�q�b�g���f�����������̂ł�������쐬
		const DsModel* pHitModel = hitRes.GetAnimRes()->CreateAnimModel();
		if (pHitModel)
		{
			DsRigidMesh::DsRigidMeshFactory factory(*pHitModel, pInsName);
			_SetActorCoord(factory, pos, ang);
			const DsActor::Option option = _GetActorOption(type);
			factory.SetOption(option);
			ret = world.CreateActor(factory);
			DsActor* pActor = world.GetActor(ret);
			if (pActor)
			{
				pActor->SetMaterial(DsActorMaterial::Aluminum());
			}
			delete pHitModel;//world.CreateActor������͂��������͂�
		}
		else
		{
			DS_LOG("�I�u�W�F�쐬���A�j�����f�������������̂ŃI�u�W�F�͍���܂���ł��� %s", pInsName);
		}
	}
	else
	{
		//�q�b�g���f�����Ȃ��̂Ō`���񂩂�쐬
		DsVec3d vertex[8];
		const DsHitRes::Shape& shape = hitRes.RefSpahe();
		DsRigidBox::GetVertex(vertex, shape.sizeX, shape.sizeY, shape.sizeZ);
		DsRigidBox::DsRigidBoxFactory factory(vertex, shape.weight, pInsName);
		_SetActorCoord(factory, pos, ang);
		const DsActor::Option option = _GetActorOption(type);
		factory.SetOption(option);

		ret = world.CreateActor(factory);
		DsActor* pActor = world.GetActor(ret);
		if (pActor)
		{
			pActor->SetMaterial(DsActorMaterial::Aluminum());
		}
	}

	return ret;
}