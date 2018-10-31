#include "DsAppPch.h"
#include "World/DsGameWorld.h"
//他のヘッダ
#include "World/System/DsGameSys.h"
#include "World/Field/Creator/DsFieldObjectCreator.h"
#include "Collision/DsCollisionCallback.h"
#include "World/Physics/DsAppCollisionCallback.h"

using namespace DsApp;
using namespace DsPhysics;


DsGameWorld::DsGameWorld(DsLib::DsSys& sys)
	: m_sys(sys)
	, m_pPhysWorld(NULL)
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

void DsGameWorld::Initialize()
{
	const bool isSuccessSetUp = DsPhysicsManager::SetUpDefault();
	if (isSuccessSetUp) {
		m_pPhysWorld = DsPhysicsManager::GetDefaultWorld();
	}

	m_pGameSys = new DsGameSys();
	DS_ASSERT(m_pGameSys, "メモリ確保失敗");
	m_pGameSys->Initialize();

	if (m_pPhysWorld) {
		m_pFieldObjectCreator = new DsFieldObjectCreator(m_sys);
		DS_ASSERT(m_pFieldObjectCreator, "メモリ確保失敗");
		const char* dataPath = "testmap.dmap";
		m_pFieldObjectCreator->Create(dataPath, *m_pPhysWorld);

		m_pCollisionCallback = new DsAppCollisionCallback();
		DS_ASSERT(m_pCollisionCallback, "メモリ確保失敗");
		m_pPhysWorld->SetCollisionCallback(m_pCollisionCallback);
	}
	
}

void DsGameWorld::Update(double dt)
{
	if (m_pPhysWorld) {
		m_pPhysWorld->Update(dt);
	}

	if (m_pFieldObjectCreator) {
		m_pFieldObjectCreator->Update(dt);
		m_pFieldObjectCreator->UpdateAllIns(dt);
	}
}