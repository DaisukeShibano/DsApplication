#ifndef _DS_SIMU_H_
#include "DsSimu.h"
#endif

#ifndef _DS_WINDOW_MANAGER_H_
#include "System/DsWindowManager.h"
#endif

#ifndef _DS_ACTOR_SELECTOR_
#include "Tool/DsActorSelector.h"
#endif

using namespace DsLib;
using namespace DsPhysics;
using namespace DsApp;

static DsSimu* s_pSimu;

class TestMainLoop : public DsMainLoop
{
public:
	virtual void Initialize(DsMainLoopArgs& args) override;
	virtual void BeforeWindowUpdate(DsMainLoopArgs& args) override;

private:
	
};


void TestMainLoop::Initialize(DsMainLoopArgs& args)
{
	const bool isSuccessSetUp = DsPhysicsManager::SetUpDefault();

	if (isSuccessSetUp)
	{
		DsPhysicsWorld* pWorld = DsPhysicsManager::GetDefaultWorld();
		if (pWorld)
		{
			const double zOffset = 5.0;
			const double yOffset = -2.0;
			//DsVec3d vertex[8];
			//DsRigidCube::GetVertex(vertex, 0.5, 0.5, 0.5);
			//{//���P
			//	DsRigidCube::DsRigidCubeFactory factory(vertex, 1.0, "���P");
			//	factory.InitPos(DsVec3d(1.2, yOffset + 4.0, zOffset));
			//	factory.InitRot(DsMat33d::RotateX(1.0));
			//	pWorld->CreateActor(factory);
			//}
			//{//���Q
			//	DsRigidCube::DsRigidCubeFactory factory(vertex, 1.0, "���Q");
			//	factory.InitPos(DsVec3d(-1.2, yOffset + 3.0, zOffset));
			//	factory.InitRot(DsMat33d::RotateX(1.0)*DsMat33d::RotateY(1.0));
			//	pWorld->CreateActor(factory);
			//}
			//for (int i = 0; i < 10; ++i)
			//{
			//	DsVec3d v[8];
			//	DsRigidCube::GetVertex(v, 0.5, 0.5, 0.5);
			//	DsRigidCube::DsRigidCubeFactory factory(v, 1.0, "���R�B");
			//	factory.InitPos(DsVec3d(-0.2, yOffset + 4.0 + (double)i*2.0, zOffset));
			//	//factory.InitRot(DsMat33d::RotateX(0)*DsMat33d::RotateY(0));
			//	pWorld->CreateActor(factory);
			//}
			//{//�n��
			//	DsVec3d vertex[8];
			//	DsRigidCube::GetVertex(vertex, 100.0, 0.5, 100.0);
			//	DsRigidCube::DsRigidCubeFactory factory(vertex, 1.0, "ground");
			//	factory.InitPos(DsVec3d(0.0, yOffset, zOffset));
			//	factory.InitRot(DsMat33d::Identity());
			//	DsActor::Option option = DsActor::Option::Static();
			//	//option.isDrawWireFrame=true;
			//	factory.SetOption(option);
			//	pWorld->CreateActor(factory);
			//}
		}
	}
}

namespace
{
	void _MoveActor(DsActor& actor, double depth, DsMainLoopArgs& args)
	{
		const DsCamera& cam = args.sys.RefCam();
		const DsVec2i drag2i = args.sys.RefMouse().GetDragMove();
		DsVec3d drag3d = args.sys.RefWindow().GetNormalizeScreenCoord(DsVec2i::ToVec2(-drag2i.x, -drag2i.y));//���ʂ������Ă�Ɖ��肵�ĉE��n�ɂȂ�悤�ɁB
		//�J�����̖ڂ̑O�Ɉړ��ʂ������Ă���
		drag3d = (cam.GetRot()*drag3d);

		//actor��xyz���ɓ��e���āA��Ԉړ��ʂ̑����������̂��̗p
		const double moveVel = static_cast<double>(tan(DegToRad(cam.GetFovy()))*depth);
		const double xdot = DsVec3d::Dot(actor.GetRotation().GetAxisX(), drag3d)*moveVel;
		const double ydot = DsVec3d::Dot(actor.GetRotation().GetAxisY(), drag3d)*moveVel;
		const double zdot = DsVec3d::Dot(actor.GetRotation().GetAxisZ(), drag3d)*moveVel;
		DsVec3d moveVec = DsVec3d::Zero();
		if (fabs(xdot) > fabs(ydot))
		{
			if (fabs(xdot) > fabs(zdot))
			{
				moveVec = actor.GetRotation().GetAxisX()*xdot;
			}
			else
			{
				moveVec = actor.GetRotation().GetAxisZ()*zdot;
			}
		}
		else
		{
			if (fabs(ydot) > fabs(zdot))
			{
				moveVec = actor.GetRotation().GetAxisY()*ydot;
			}
			else
			{
				moveVec = actor.GetRotation().GetAxisZ()*zdot;
			}
		}
		actor.SetPosition(actor.GetPosition() + moveVec);
	}

	void _CamUpdate(DsMainLoopArgs& args)
	{
		DsCamera& cam = args.sys.RefCam();
		DsMouse& mouse = args.sys.RefMouse();
		const DsVec2i drag2i = args.sys.RefMouse().GetDragMove();
		const DsVec3d drag3 = args.sys.RefWindow().GetNormalizeScreenCoord(DsVec2i::ToVec2(-drag2i.x, -drag2i.y));//���ʂ������Ă�Ɖ��肵�ĉE��n�ɂȂ�悤�ɁB

		//�����N���b�N�Ńh���b�O�Ŏ��_�ړ�
		if (mouse.GetClickState() == DsMouseClickState::DS_CENTER_CLICK)
		{
			const float moveVel = 100.0f;
			const DsVec3d x = cam.GetRot().GetAxisX() * drag3.x * moveVel;
			const DsVec3d y = cam.GetRot().GetAxisY() * drag3.y * moveVel;
			const DsVec3d pos = cam.GetPos() + x + y;
			cam.SetPos(pos);
		}

		//�z�C�[���őO��ړ�
		{
			const int wheel = mouse.GetWheelMove();
			if (wheel != 0)
			{
				const float moveVel = 3.0f * static_cast<float>(wheel);
				const DsVec3d z = cam.GetRot().GetAxisZ() * moveVel;
				const DsVec3d pos = cam.GetPos() + z;
				cam.SetPos(pos);
			}
		}

		//�E�N���b�N�ŉ�]
		if (mouse.GetClickState() == DsMouseClickState::DS_RIGHT_CLICK)
		{
			const double moveVel = 10.0f;
			DsMat33d mat = cam.GetRot();
			const DsVec3d rotAxisX = mat.GetAxisX();
			const DsVec3d rotAxisY = mat.GetAxisY();
			mat = DsMat33d::RotateVec(rotAxisX*drag3.y*moveVel*(-1.0f)) * mat;
			mat = DsMat33d::RotateVec(rotAxisY*drag3.x*moveVel) * mat;
			cam.SetRot(mat);
		}
	}
}


void TestMainLoop::BeforeWindowUpdate(DsMainLoopArgs& args)
{
	DsPhysicsWorld* pWorld = DsPhysicsManager::GetDefaultWorld();
	if (pWorld)
	{
		pWorld->Update(args.dt);
	
		//�}�E�X�őI������
		double depth;
		DsActorId actorId = DsActorSelector::Select(*pWorld, &depth);
		DsActor* pSelectActor = pWorld->GetActor(actorId);
		for each(DsActor* actor in pWorld->GetActors())
		{
			if (pSelectActor == actor)
			{
				actor->SetColor(DsVec4d(0.2, 0.8, 0.8, 1));
				actor->SetLineColor(DsVec4d(0, 0, 0, 1));
				actor->Draw(args.drawCom);
				args.drawCom.SetColor(DsVec3d::ToVec3(1.0, 0.8, 0.8));
				actor->RefAabb().Draw(args.drawCom);
				args.drawCom.DrawAxis(actor->GetRotation(), actor->GetPosition(), actor->RefAabb().GetMaxLen()*2.0);
			}
			else
			{
				//args.drawCom.SetColor(DsVec3d::ToVec3(1.0, 1.0, 1.0));
			}
			
		}

		if (pSelectActor)
		{
			_MoveActor(*pSelectActor, depth, args);
		}
	}

	_CamUpdate(args);

	if (DsSimu::GetIns())
	{
		DsSimu::GetIns()->Update(args.dt);
	}

}



//static 
void DsSimu::Initialize()
{
	s_pSimu = new DsSimu();
}

//static 
void DsSimu::Finalize()
{
	delete s_pSimu;
	s_pSimu = NULL;
}

//static 
DsSimu* DsSimu::GetIns()
{
	return s_pSimu;
}


DsSimu::DsSimu()
	:m_pSys(NULL)
	,m_pLoop(NULL)
	,m_isInit(false)
	,m_fieldObjs()
	,m_resource()
{
	
}

DsSimu::~DsSimu()
{
	delete m_pSys;
	m_pSys = NULL;
	delete m_pLoop;
	m_pLoop = NULL;
	for each(DsFieldObj* obj in m_fieldObjs)
	{
		delete obj;
	}
	m_fieldObjs.clear();
}


void DsSimu::Setup(int handle)
{
	m_pSys = new DsSys();
	m_pLoop = new TestMainLoop();
	DsSysArgs args;
	args.argc = 0;
	args.argv = NULL;
	args.pLoop = m_pLoop;
	args.pConfPath = "config.txt";
	args.windowType = DS_WINDOW_SYSTEM_TYPE_GL;
	args.windowHandle = handle;
	m_pSys->Setup(args);
	m_isInit = true;
}

DsWindowGL& DsSimu::RefWindow()
{
	return static_cast<DsWindowGL&>(m_pSys->RefWindow());
}

void DsSimu::Update(double dt)
{
	for each(DsFieldObj* pObj in m_fieldObjs)
	{
		pObj->Update(dt);
		DsDbgSys::GetIns().RefDrawCom().SetColor(0.9, 1.0, 0.9);
		pObj->DbgDraw(DsDbgSys::GetIns().RefDrawCom());
	}
}

DsFieldObj* DsSimu::RegisterObj(const char* drawModelPath, const char* hitModelPath, double px, double py, double pz, double rx, double ry, double rz, DS_MAP_OBJ_TYPE physicsType, DS_MAP_FIELD_OBJ_TYPE fieldObjType)
{
	DsAnimRes* pAnim = m_resource.RegisterAnimRes(drawModelPath);
	DsHitRes* pHit = m_resource.RegisterHitRes(hitModelPath);

	DsFieldObj* pObj = NULL;

	switch (fieldObjType)
	{
	case DsLib::DS_MAP_FIELD_OBJ_TYPE::CHR:
	case DsLib::DS_MAP_FIELD_OBJ_TYPE::PLAYER:
		pObj = new DsFieldChr(*m_pSys, *DsPhysicsManager::GetDefaultWorld());
		break;
	case DsLib::DS_MAP_FIELD_OBJ_TYPE::OBJ:
		pObj = new DsFieldObj(*m_pSys, *DsPhysicsManager::GetDefaultWorld());
		break;
	case DsLib::DS_MAP_FIELD_OBJ_TYPE::HIT:
		pObj = new DsFieldHit(*m_pSys, *DsPhysicsManager::GetDefaultWorld());
		break;
	default:
		pObj = new DsFieldObj(*m_pSys, *DsPhysicsManager::GetDefaultWorld());
		break;
	}
	
	DsFieldObj::InitInfo info;
	info.name = drawModelPath;
	info.pos = DsVec3d(px, py, pz);
	info.rot = DsMat33d::RotateZ(DegToRad(rz))*DsMat33d::RotateY(DegToRad(ry))*DsMat33d::RotateX(DegToRad(rx));
	info.pHitRes = pHit;
	info.pAnimRes = pAnim;
	info.physicsType = physicsType;
	pObj->Initialize(info);

	if ((fieldObjType == DsLib::DS_MAP_FIELD_OBJ_TYPE::CHR) || (fieldObjType == DsLib::DS_MAP_FIELD_OBJ_TYPE::OBJ) || (fieldObjType == DsLib::DS_MAP_FIELD_OBJ_TYPE::PLAYER))
	{
		//�`�悪����Č��ɂ����̂Ń��C���[�t���[���\��
		pObj->GetActor()->RefOption().isDrawWireFrame = true;
	}
	pObj->GetActor()->RefOption().isGravity = false;
	pObj->GetActor()->RefOption().isStatic = true;

	m_fieldObjs.push_back(pObj);
	return pObj;
}

void DsSimu::Unregister(DsApp::DsFieldObj* pObj)
{
	for each(DsFieldObj* obj in m_fieldObjs)
	{
		if (obj == pObj)
		{
			m_fieldObjs.remove(pObj);
			delete pObj;
			break;
		}
	}
}

void DsSimu::ClearObj()
{
	for each(DsFieldObj* pObj in m_fieldObjs)
	{
		delete pObj;
	}
	m_fieldObjs.clear();
}