#include "DsAppPch.h"
#include "World/Field/DsFieldEnemy.h"
//���̃w�b�_
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
void DsFieldEnemy::Update1(double dt)
{
	DsFieldChr::Update1(dt);
}
//virtual
void DsFieldEnemy::Update2(double dt)
{
	//TODO:���񂾂烍�b�N�I���؂�
	DsComponentSystem* pComSys = GetComponentSystem();
	if (pComSys) {
		pComSys->RequestLockOnPoint();
	}

	DsFieldChr::Update2(dt);
}
//virtual
void DsFieldEnemy::Update3(double dt)
{
	DsFieldChr::Update3(dt);
}