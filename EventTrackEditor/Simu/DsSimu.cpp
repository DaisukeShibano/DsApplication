#include "stdafx.h"

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
	DsAppCollisionCallback m_callback;
	DsActorId m_ground;
};


void TestMainLoop::Initialize(DsMainLoopArgs& args)
{
	const bool isSuccessSetUp = DsPhysicsManager::SetUpDefault();

	if (isSuccessSetUp)
	{
		DsPhysicsWorld* pWorld = DsPhysicsManager::GetDefaultWorld();
		if (pWorld)
		{
			pWorld->SetCollisionCallback(&m_callback);
			DsVec3d vertex[8];
			DsRigidBox::GetVertex(vertex, 10.0, 1.0, 10.0);
			{//地面
				DsRigidBox::DsRigidBoxFactory factory(vertex, 1.0, "地面");
				factory.InitPos(DsVec3d(0.0, -0.5, 0.0));
				m_ground = pWorld->CreateActor(factory);
				m_ground.GetActor()->RefOption().isStatic = true;
			}
		}
	}

	args.sys.RefCam().SetPos(DsVec3d(0, 2, -2));
	args.sys.RefCam().SetRot(DsMat33d::RotateX(DegToRad(30.0)));
}

namespace
{
	void _CamUpdate(DsMainLoopArgs& args)
	{
		DsCamera& cam = args.sys.RefCam();
		DsMouse& mouse = args.sys.RefMouse();
		const DsVec2i drag2i = args.sys.RefMouse().GetDragMove();
		const DsVec3d drag3 = args.sys.RefWindow().GetNormalizeScreenCoord(DsVec2i::ToVec2(-drag2i.x, -drag2i.y));//正面を向いてると仮定して右手系になるように。

		//ホイールで前後移動
		{
			const int wheel = mouse.GetWheelMove();
			if (wheel != 0)
			{
				const float moveVel = 0.5f * static_cast<float>(wheel);
				const DsVec3d z = cam.GetRot().GetAxisZ() * moveVel;
				const DsVec3d pos = cam.GetPos() + z;
				cam.SetPos(pos);
			}
		}

		//左クリックで回転
		if (mouse.GetClickState() == DsMouseClickState::DS_LEFT_CLICK)
		{
			const double moveVel = 2.0f;
			DsMat33d mat = cam.GetRot();
			const DsVec3d rotAxisX = mat.GetAxisX();
			const DsVec3d rotAxisY = mat.GetAxisY();
			const DsMat33d rotMatY = DsMat33d::RotateVec(DsVec3d::GetY()*drag3.x*moveVel);
			const DsMat33d rotMatX = rotMatY * DsMat33d::RotateVec(rotAxisX*drag3.y*moveVel*(-1.0));
			const DsMat33d rotMat = rotMatY * rotMatX;
			cam.SetRot(rotMat*mat);

			const DsVec3d lookPos = DsVec3d(0, 0, 0);
			const DsVec3d lookVec = (cam.GetPos()-lookPos);
			const DsVec3d lookVecR = rotMat *lookVec;
			const DsVec3d movePos = lookVecR - lookVec;
			cam.SetPos(cam.GetPos() + movePos);
		}
	}
}


void TestMainLoop::BeforeWindowUpdate(DsMainLoopArgs& args)
{
	DsPhysicsWorld* pWorld = DsPhysicsManager::GetDefaultWorld();
	if (pWorld)
	{
		pWorld->Update(args.dt);
	}

	_CamUpdate(args);

	if (DsSimu::GetIns())
	{
		DsSimu::GetIns()->Update(args.dt);
	}

	m_ground.GetActor()->SetColor(DsVec4d(0.4, 0.4, 0.4, 1.0));
	m_ground.GetActor()->Draw(args.drawCom);
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
	,m_fieldObjs()
	,m_selectAnimName()
	,m_pChrIns(NULL)
	,m_isInit(false)

{
	
}

DsSimu::~DsSimu()
{
	for(DsFieldObj* obj : m_fieldObjs)
	{
		delete obj;
	}
	m_fieldObjs.clear();
	delete m_pLoop;
	m_pLoop = NULL;
	delete m_pSys;
	m_pSys = NULL;

}


void DsSimu::Setup(ds_uint64 handle)
{
	//DsPath::SetAssetPath(DsPath::GetCurrentPath() + L"\\..\\Test\\Asset");
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
	if (m_pChrIns) {
		m_pChrIns->SetRequestAnim(m_selectAnimName);
		m_pChrIns->Update(dt);
	}
}

std::vector<std::string> DsSimu::RegisterObj(const char* drawModelPath, const char* hitModelPath, double px, double py, double pz, double rx, double ry, double rz, DS_MAP_OBJ_TYPE physicsType, DS_MAP_FIELD_OBJ_TYPE fieldObjType)
{
	std::vector<std::string> ret;

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
	
	DsFieldInitInfo info;
	info.name = drawModelPath;
	info.pos = DsVec3d(px, py, pz);
	info.ang = DsVec3d(DegToRad(rx), DegToRad(ry), DegToRad(rz));
	info.hitName = hitModelPath;
	info.animName = drawModelPath;
	info.physicsType = physicsType;
	pObj->Initialize(info);

	pObj->GetActor()->RefOption().isGravity = false;
	pObj->GetActor()->RefOption().isStatic = true;

	m_fieldObjs.push_back(pObj);
	m_pChrIns = pObj;

	if (m_pChrIns) {
		DsAnimation* pAnim = m_pChrIns->GetAnim();
		if (pAnim) {
			for( DsAnimClip* pClip : pAnim->RefAnimClips() ){
				ret.push_back(pClip->RefAnimName());
			}
		}
	}

	return ret;
}

void DsSimu::ClearObj()
{
	m_pChrIns = NULL;
	for(DsFieldObj* pObj : m_fieldObjs)
	{
		delete pObj;
	}
	m_fieldObjs.clear();
}

void DsSimu::SetLocalTimeAnim(double time)
{
	if (m_pChrIns) {
		DsAnimation* pAnim = m_pChrIns->GetAnim();
		if (pAnim) {
			for (DsAnimClip* pClip : pAnim->RefAnimClips()) {
				if (m_selectAnimName == pClip->RefAnimName()) {
					pClip->SetLocalTime(time);
				}
				else {
					//選択アニメ以外は元に戻しておく
					pClip->SetLocalTime(0);
				}
			}
		}
	}

}

void DsSimu::SetCurrentAnimName(const char* name)
{
	m_selectAnimName = name;
}
std::string DsSimu::GetCurrentAnimName()const
{
	return m_selectAnimName;
}

bool DsSimu::IsEndAnim()const
{
	bool ret = true;
	if (m_pChrIns) {
		DsAnimation* pAnim = m_pChrIns->GetAnim();
		if (pAnim) {
			for (DsAnimClip* pClip : pAnim->RefAnimClips()) {
				if (m_selectAnimName == pClip->RefAnimName()) {
					ret = pClip->IsEndWithoutBlend();
					break;
				}
			}
		}
	}
	return ret;
}
