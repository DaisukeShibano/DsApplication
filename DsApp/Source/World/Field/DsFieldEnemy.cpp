#include "DsAppPch.h"
#include "World/Field/DsFieldEnemy.h"
//他のヘッダ
#include "World/Component/DsComponentSystem.h"


using namespace DsLib;
using namespace DsPhysics;
using namespace DsApp;


DsFieldEnemy::DsFieldEnemy(DsLib::DsSys& sys, DsPhysics::DsPhysicsWorld& world)
	:DsFieldChr(sys, world)
{

}

DsFieldEnemy::~DsFieldEnemy()
{

}

//virtual
void DsFieldEnemy::Initialize(const DsFieldInitInfo& initInfo)
{
	DsFieldChr::Initialize(initInfo);

}


//virtual
void DsFieldEnemy::Update(double dt)
{
	//TODO:死んだらロックオン切る
	DsComponentSystem* pComSys = GetComponentSystem();
	if (pComSys) {
		pComSys->RequestLockOnPoint();
	}

	DsFieldChr::Update(dt);
}