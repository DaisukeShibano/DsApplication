#include "DsAppPch.h"
#include "World/System/DsGameSys.h"
//���̃w�b�_
#include "World/Field/LockOn/DsLockOn.h"
#include "World/Field/HitStop/DsHitStop.h"


using namespace DsApp;

DsGameSys::DsGameSys()
	: m_pLockOn(NULL)
	, m_pHitStop(NULL)
{
}

DsGameSys::~DsGameSys()
{
	delete m_pLockOn; m_pLockOn = NULL;
	delete m_pHitStop; m_pHitStop = NULL;
}

void DsGameSys::Initialize()
{
	m_pLockOn = new DsLockOn();
	DS_ASSERT(m_pLockOn, "�������m�ێ��s");

	m_pHitStop = new DsHitStop();
	DS_ASSERT(m_pHitStop, "�������m�ێ��s");
}