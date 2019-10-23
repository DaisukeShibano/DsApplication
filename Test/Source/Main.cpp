#include "TestPch.h"
#ifndef _DS_WINDOW_MANAGER_H_
#include "System/DsWindowManager.h"
#endif

#include<windows.h>

using namespace DsApp;

class TestMainLoop : public DsMainLoop
{
public:
	TestMainLoop()
		: DsMainLoop()
		//, m_pCreator(NULL)
		, m_gameWorld()
	{

	}

	~TestMainLoop()
	{
		//delete m_pCreator;
		//m_pCreator = NULL;
	}

public:
	virtual void Initialize(DsMainLoopArgs& args) override;
	virtual void BeforeWindowUpdate(DsMainLoopArgs& args) override;

private:
	DsGameWorld m_gameWorld;

private:
	//DsFieldObjectCreator* m_pCreator;
	//DsHinge2Joint* m_joint;
	//DsHinge2Joint* m_joint2;
	//DsAppCollisionCallback m_callback;
	//
	//DsCamera m_cam;
	//DsRenderCamCaptureImage* m_pImage;
	//DsCamera m_cam2;
	//DsRenderCamCaptureImage* m_pImage2;
};


void TestMainLoop::Initialize(DsMainLoopArgs& args)
{
	//const bool isSuccessSetUp = DsPhysicsManager::SetUpDefault();
	//
	//if (isSuccessSetUp)
	//{
	//	DsPhysicsWorld* pWorld = DsPhysicsManager::GetDefaultWorld();
	//	if (pWorld)
	//	{
	//		pWorld->SetCollisionCallback(&m_callback);
	//
	//		DsActorId act1;
	//		DsActorId act2;
	//		DsActorId act3;
	//		DsVec3d vertex[8];
	//		DsActor::Option option = DsActor::Option::Default();
	//		//option.isGravity = false;
	//
	//		m_joint = new DsHinge2Joint(*pWorld);
	//		m_joint2 = new DsHinge2Joint(*pWorld);
	//		if(0){//ÉWÉáÉCÉìÉgÉeÉXÉg
	//			DsRigidBox::GetVertex(vertex, 0.5, 0.5, 0.5);
	//			{//î†ÇP
	//				DsRigidBox::DsRigidBoxFactory factory(vertex, 1.0, "î†ÇP");
	//				factory.InitPos(DsVec3d(2.0, 1.0, 5.0));
	//				factory.SetOption(option);
	//				act1 = pWorld->CreateActor(factory);
	//				act1.GetActor()->SetMaterial(DsActorMaterial::Aluminum());
	//			}
	//			{//î†ÇQ
	//				DsRigidBox::DsRigidBoxFactory factory(vertex, 1.0, "î†ÇQ");
	//				factory.InitPos(DsVec3d(1.0, 1.0, 5.0));
	//				factory.SetOption(option);
	//				act2 = pWorld->CreateActor(factory);
	//				act2.GetActor()->SetMaterial(DsActorMaterial::Aluminum());
	//			}
	//			{//î†ÇR
	//				DsRigidBox::DsRigidBoxFactory factory(vertex, 1.0, "î†ÇR");
	//				factory.InitPos(DsVec3d(0.0, 1.0, 5.0));
	//				factory.SetOption(option);
	//				act3 = pWorld->CreateActor(factory);
	//				act3.GetActor()->SetMaterial(DsActorMaterial::Aluminum());
	//			}
	//			m_joint->AttachJoint(act1, act2, act1.GetActor()->GetPosition() + DsVec3d(-0.5, 0, 0), DsVec3d(0,0,1), DsVec3d(1, 0, 0));
	//			m_joint2->AttachJoint(act2, act3, act2.GetActor()->GetPosition() + DsVec3d(-0.5, 0, 0), DsVec3d(0, 0, 1), DsVec3d(1, 0, 0));
	//		}
	//
	//		if(1){//è’ìÀÉeÉXÉg
	//			DsRigidBox::GetVertex(vertex, 1.5, 0.5, 1.5);
	//			for(int i=0; i<0; ++i){
	//				DsRigidBox::DsRigidBoxFactory factory(vertex, 1.0, "î†ÇP");
	//				factory.InitPos(DsVec3d(0.0, 0.0 + (0.5)*(double)i, 2.0));
	//				factory.SetOption(option);
	//				DsActorId actorId = pWorld->CreateActor(factory);
	//				DsActor* pActor = actorId.GetActor();
	//				pActor->SetMaterial(DsActorMaterial::Aluminum());
	//			}
	//			for (int i = 0; i<0; ++i) {
	//				DsRigidCapsule::DsRigidCapsuleFactory factory(0.5, 1.0, 1000.0, "î†ÇP");
	//				factory.InitPos(DsVec3d(-1.0, 1.6 + (2.0)*(double)i, 4.0));
	//				factory.InitRot(DsMat33d::RotateZ(0.3));
	//				factory.SetOption(option);
	//				DsActorId actorId = pWorld->CreateActor(factory);
	//				DsActor* pActor = actorId.GetActor();
	//				pActor->SetMaterial(DsActorMaterial::Aluminum());
	//			}
	//		}
	//
	//		const char* dataPath = "testmap.dmap";
	//		m_pCreator = new DsFieldObjectCreator(args.sys);
	//		m_pCreator->Create(dataPath, *pWorld);
	//	}
	//}
	//
	//const DsCamera& sysCam = args.sys.RefCam();
	//m_cam = sysCam;
	//m_pImage = DsRenderCamCaptureImage::Create(m_cam, 128, 128);
	//args.sys.RefRender().RegisterCaptureImage(m_pImage);
	//
	//m_cam2 = sysCam;
	//m_pImage2 = DsRenderCamCaptureImage::Create(m_cam2, 128, 128);
	//args.sys.RefRender().RegisterCaptureImage(m_pImage2);


	m_gameWorld.Initialize(args.sys);
}


#include <regex>
void func()
{
	struct ENV_DATA
	{
		std::string prefix;
		std::string env;
		std::string suffix;
		int envId;
		int type;
	};

	std::vector<ENV_DATA> result;


	std::map<int, const char*> nameMap;
	nameMap[123] = "TEST";
	nameMap[45] = "DATA";

	const char* data = "hoge_if(env(123) == 1) && push_cache_0 && env(45) == 1";

	std::regex re0("(.*?)(env\\((\\d+)\\))(.*(?=env))");
	std::cmatch match0;

	std::string search0 = data;
	while (std::regex_search(search0.c_str(), match0, re0, std::regex_constants::match_default)) {

		if (match0.size() == 5) {
			ENV_DATA data0;

			//ï∂ì™Ç…pushÇ™óàÇΩèÍçáÇ…ëŒâûÇ≈Ç´Ç»Ç¢

			data0.prefix = match0[1].str();
			data0.env = match0[2].str();
			data0.envId = std::atoi(match0[3].str().c_str());
			data0.suffix = match0[4].str();
			data0.type = 0;
			result.push_back(data0);

			std::regex re1("(.*?)(push_cache_\\d+)");
			std::cmatch match1;
			std::string search1 = match0[0].str();
			bool setPrefix = true;
			while (std::regex_search(search1.c_str(), match1, re1, std::regex_constants::match_default)) {

				if (match1.size() == 3) {
					ENV_DATA data1;

					data1.prefix = match1[1].str();
					data1.env = match1[2].str();
					data1.envId = -1;
					data1.type = 1;

					//ÉTÉtÉBÉbÉNÉXÇpush_cache_íºëOÇ…çXêV
					if (setPrefix) {
						result.back().suffix = data1.prefix;
						setPrefix = false;
					}

					result.push_back(data1);
				}


				search1 = match1.suffix().str();
			}
		}

		search0 = match0.suffix().str();
	}
}


void TestMainLoop::BeforeWindowUpdate(DsMainLoopArgs& args)
{
	m_gameWorld.Update(args.dt);
	
	//DsPhysicsWorld* pWorld = DsPhysicsManager::GetDefaultWorld();
	//if (pWorld) {
	//	const DsPhysicsWorld::Actors& actors = pWorld->GetActors();
	//	for (DsActor* pActor : actors) {
	//		args.drawCom.SetColor(DsVec3d(1, 0, 0));
	//		pActor->Draw(args.drawCom);
	//	}
	//}


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
