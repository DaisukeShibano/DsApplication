#include "TestPch.h"
#ifndef _DS_WINDOW_MANAGER_H_
#include "System/DsWindowManager.h"
#endif

#include<windows.h>


class TestMainLoop : public DsMainLoop
{
public:
	TestMainLoop()
		: DsMainLoop()
		, m_pCreator(NULL)
	{

	}

	~TestMainLoop()
	{
		delete m_pCreator;
		m_pCreator = NULL;
	}

public:
	virtual void Initialize(DsMainLoopArgs& args) override;
	virtual void BeforeWindowUpdate(DsMainLoopArgs& args) override;

private:
	DsFieldObjectCreator* m_pCreator;
	DsHinge2Joint* m_joint;
	DsHinge2Joint* m_joint2;
	DsAppCollisionCallback m_callback;

	DsCamera m_cam;
	DsRenderCamCaptureImage* m_pImage;
	DsCamera m_cam2;
	DsRenderCamCaptureImage* m_pImage2;
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

			DsActorId act1;
			DsActorId act2;
			DsActorId act3;
			DsVec3d vertex[8];
			DsActor::Option option = DsActor::Option::Default();
			//option.isGravity = false;

			m_joint = new DsHinge2Joint(*pWorld);
			m_joint2 = new DsHinge2Joint(*pWorld);
			if(0){//ÉWÉáÉCÉìÉgÉeÉXÉg
				DsRigidBox::GetVertex(vertex, 0.5, 0.5, 0.5);
				{//î†ÇP
					DsRigidBox::DsRigidBoxFactory factory(vertex, 1.0, "î†ÇP");
					factory.InitPos(DsVec3d(2.0, 1.0, 5.0));
					factory.SetOption(option);
					act1 = pWorld->CreateActor(factory);
					act1.GetActor()->SetMaterial(DsActorMaterial::Aluminum());
				}
				{//î†ÇQ
					DsRigidBox::DsRigidBoxFactory factory(vertex, 1.0, "î†ÇQ");
					factory.InitPos(DsVec3d(1.0, 1.0, 5.0));
					factory.SetOption(option);
					act2 = pWorld->CreateActor(factory);
					act2.GetActor()->SetMaterial(DsActorMaterial::Aluminum());
				}
				{//î†ÇR
					DsRigidBox::DsRigidBoxFactory factory(vertex, 1.0, "î†ÇR");
					factory.InitPos(DsVec3d(0.0, 1.0, 5.0));
					factory.SetOption(option);
					act3 = pWorld->CreateActor(factory);
					act3.GetActor()->SetMaterial(DsActorMaterial::Aluminum());
				}
				m_joint->AttachJoint(act1, act2, act1.GetActor()->GetPosition() + DsVec3d(-0.5, 0, 0), DsVec3d(0,0,1), DsVec3d(1, 0, 0));
				m_joint2->AttachJoint(act2, act3, act2.GetActor()->GetPosition() + DsVec3d(-0.5, 0, 0), DsVec3d(0, 0, 1), DsVec3d(1, 0, 0));
			}

			if(1){//è’ìÀÉeÉXÉg
				DsRigidBox::GetVertex(vertex, 1.5, 0.5, 1.5);
				for(int i=0; i<0; ++i){
					DsRigidBox::DsRigidBoxFactory factory(vertex, 1.0, "î†ÇP");
					factory.InitPos(DsVec3d(0.0, 0.0 + (0.5)*(double)i, 2.0));
					factory.SetOption(option);
					DsActorId actorId = pWorld->CreateActor(factory);
					DsActor* pActor = actorId.GetActor();
					pActor->SetMaterial(DsActorMaterial::Aluminum());
				}
				for (int i = 0; i<3; ++i) {
					DsRigidCapsule::DsRigidCapsuleFactory factory(0.5, 1.0, 1000.0, "î†ÇP");
					factory.InitPos(DsVec3d(-1.0, 1.6 + (2.0)*(double)i, 4.0));
					factory.InitRot(DsMat33d::RotateZ(0.3));
					factory.SetOption(option);
					DsActorId actorId = pWorld->CreateActor(factory);
					DsActor* pActor = actorId.GetActor();
					pActor->SetMaterial(DsActorMaterial::Aluminum());
				}
			}

			const char* dataPath = "testmap.dmap";
			m_pCreator = new DsFieldObjectCreator(args.sys);
			m_pCreator->Create(dataPath, *pWorld);
		}
	}

	//const DsCamera& sysCam = args.sys.RefCam();
	//m_cam = sysCam;
	//m_pImage = DsRenderCamCaptureImage::Create(m_cam, 128, 128);
	//args.sys.RefRender().RegisterCaptureImage(m_pImage);
	//
	//m_cam2 = sysCam;
	//m_pImage2 = DsRenderCamCaptureImage::Create(m_cam2, 128, 128);
	//args.sys.RefRender().RegisterCaptureImage(m_pImage2);

}

void TestMainLoop::BeforeWindowUpdate(DsMainLoopArgs& args)
{
	DsPhysicsWorld* pWorld = DsPhysicsManager::GetDefaultWorld();
	if (pWorld)
	{
		pWorld->Update(args.dt);
		int color = 0;
		for(DsActor* actor : pWorld->GetActors())
		{
			std::string tmp = actor->GetName();
			//if( (tmp != "robo_0") && (tmp!="ragdoll") )
			if ((tmp == "î†ÇP") || (tmp == "î†ÇQ") || (tmp == "ChrProxy"))
			//if (actor->GetType() == DsActor::RIGID_CAPSULE)
			{
				const double rC = (actor->IsRest()) ? (0.5) : (1.0);
				if (color == 0) {
					actor->SetColor(DsVec4d(1.0, 0.6, 0.2, 1)*rC);
				}
				else if (color == 1) {
					actor->SetColor(DsVec4d(0.2, 1.0, 0.6, 1)*rC);
				}
				else {
					actor->SetColor(DsVec4d(0.4, 0.6, 1.0, 1)*rC);
				}
				actor->SetLineColor(DsVec4d(0, 0, 0, 1));
				actor->Draw(args.drawCom);
			}
			++color;
			color %= 3;
		}
		m_joint->Update(args.dt);
		m_joint->DbgDraw(args.drawCom);

		m_joint2->Update(args.dt);
		m_joint2->DbgDraw(args.drawCom);

	}

	m_pCreator->Update(args.dt);

	for(DsFieldHit* obj : m_pCreator->RefHits())
	{
		obj->Update(args.dt);
	}
	for(DsFieldChr* obj : m_pCreator->RefChrs())
	{
		obj->Update(args.dt);
	}
	for(DsFieldObj* obj : m_pCreator->RefObjs())
	{
		obj->Update(args.dt);
	}

	//const DsCamera& sysCam = args.sys.RefCam();
	//m_cam = sysCam;
	//m_cam.SetRot(DsMat33d::RotateY(RadToDeg(10))*sysCam.GetRot());
	//m_cam2 = sysCam;
	//m_cam2.SetRot(DsMat33d::RotateY(RadToDeg(-10))*sysCam.GetRot());
	//
	//DsVec3d poly[] =
	//{
	//	{ 1+3,-1,0},
	//	{ -1+3,-1,0 },
	//	{ -1+3,1,0 },
	//	{ 1+3,1,0 },
	//};
	//DsVec3d poly2[] =
	//{
	//	{ 1-1,-1,0 },
	//	{ -1-1,-1,0 },
	//	{ -1-1,1,0 },
	//	{ 1-1,1,0 },
	//};
	//DsVec2d tex[] =
	//{
	//	{ 0,0},
	//	{ 1,0},
	//	{ 1,1},
	//	{ 0,1},
	//};
	//
	//args.drawCom.DrawTexQuad(poly[0], poly[1], poly[2], poly[3], 
	//	tex[0], tex[1], tex[2], tex[3],
	//	m_pImage->GetImage(), m_pImage->GetWidth(), m_pImage->GetHeight());
	//args.drawCom.DrawTexQuad(poly2[0], poly2[1], poly2[2], poly2[3],
	//	tex[0], tex[1], tex[2], tex[3],
	//	m_pImage2->GetImage(), m_pImage2->GetWidth(), m_pImage2->GetHeight());
	
	DsPerf::RefPerfTotalFps().DbgDrawFps();

	//args.drawCom.SetColor(DsVec3d(0, 0, 0)).DrawTextScreen(DsVec2d(-0.33, 0.36), L" w");
	//args.drawCom.SetColor(DsVec3d(0, 0, 0)).DrawTextScreen(DsVec2d(-0.33, 0.32), L"a d  à⁄ìÆ");
	//args.drawCom.SetColor(DsVec3d(0, 0, 0)).DrawTextScreen(DsVec2d(-0.33, 0.28), L" s");
	//args.drawCom.SetColor(DsVec3d(0, 0, 0)).DrawTextScreen(DsVec2d(-0.33, 0.20), L"drag éãì_âÒì]");
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
	ds_uint64 hwnd = DsWindowManager::MainWindowCreate((ds_uint64)hInstance, lpCmdLine, nCmdShow);
	if (0 == hwnd) {
		return 0;
	}
	DsPath::SetAssetPath(DsPath::GetCurrentPath() + L"\\Asset");
	DsSys* pSys = new DsSys();
	DS_ASSERT(pSys, "ÉÅÉÇÉäämï€é∏îs");
	TestMainLoop* pLoop = new TestMainLoop();
	DS_ASSERT(pLoop, "ÉÅÉÇÉäämï€é∏îs");
	DsSysArgs args;
	args.argc = 0;
	args.argv = &lpCmdLine;
	args.pLoop = pLoop;
	args.pConfPath = "config.txt";
	args.windowType = DS_WINDOW_SYSTEM_TYPE_GL;
	args.windowHandle = hwnd;
	pSys->Setup(args);

	while(DsWindowManager::MainWindowUpdate(hwnd, *pSys));
	
	delete pLoop;
	delete pSys;
	return 0;
}
