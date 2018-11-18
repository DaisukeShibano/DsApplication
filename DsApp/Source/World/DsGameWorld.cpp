#include "DsAppPch.h"
#include "World/DsGameWorld.h"
//‘¼‚Ìƒwƒbƒ_
#include "World/System/DsGameSys.h"
#include "World/Field/Creator/DsFieldObjectCreator.h"
#include "Collision/DsCollisionCallback.h"
#include "World/Physics/DsAppCollisionCallback.h"
#include "World/Field/Action/DsChrStateDefine.h"
#include "World/Field/HitStop/DsHitStop.h"


using namespace DsApp;
using namespace DsPhysics;


DsGameWorld::DsGameWorld()
	: m_pPhysWorld(NULL)
	, m_pGameSys(NULL)
	, m_pFieldObjectCreator(NULL)
	, m_pCollisionCallback(NULL)
{

}

DsGameWorld::~DsGameWorld()
{
	delete m_pGameSys; m_pGameSys = NULL;
	delete m_pFieldObjectCreator; m_pFieldObjectCreator = NULL;
	delete m_pCollisionCallback; m_pCollisionCallback = NULL;
}

void DsGameWorld::Initialize(DsLib::DsSys& sys)
{
	const bool isSuccessSetUp = DsPhysicsManager::SetUpDefault();
	if (isSuccessSetUp) {
		m_pPhysWorld = DsPhysicsManager::GetDefaultWorld();
	}

	m_pGameSys = new DsGameSys();
	DS_ASSERT(m_pGameSys, "ƒƒ‚ƒŠŠm•ÛŽ¸”s");
	m_pGameSys->Initialize();

	//ƒLƒƒƒ‰ƒXƒe[ƒg‚ÌƒNƒ‰ƒX“o˜^‚È‚Ç
	DsChrStateDefine::InitializeState();

	if (m_pPhysWorld) {
		m_pFieldObjectCreator = new DsFieldObjectCreator(sys);
		DS_ASSERT(m_pFieldObjectCreator, "ƒƒ‚ƒŠŠm•ÛŽ¸”s");
		const char* dataPath = "testmap.dmap";
		m_pFieldObjectCreator->Create(dataPath, *m_pPhysWorld, m_pGameSys);

		m_pCollisionCallback = new DsAppCollisionCallback();
		DS_ASSERT(m_pCollisionCallback, "ƒƒ‚ƒŠŠm•ÛŽ¸”s");
		m_pPhysWorld->SetCollisionCallback(m_pCollisionCallback);
	}
	
}

void DsGameWorld::Update(double dt)
{
	bool isUpdateIns = true;

	if (m_pPhysWorld) {
		m_pPhysWorld->Update(dt);
	}

	if (m_pGameSys) {
		DsHitStop* pHitStop = m_pGameSys->GetHitStop();
		if (pHitStop) {
			pHitStop->Update(dt);
			if (pHitStop->IsHitStop()) {
				isUpdateIns = false;
			}
		}
	}

	if (m_pFieldObjectCreator) {
		m_pFieldObjectCreator->Update(dt);

		if (isUpdateIns) {
			m_pFieldObjectCreator->UpdateAllIns(dt);
		}
	}
}