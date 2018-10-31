#include "DsAppPch.h"
#include "World/System/DsGameSys.h"
//ëºÇÃÉwÉbÉ_
#include "World/Field/LockOn/DsLockOn.h"


using namespace DsApp;

DsGameSys::DsGameSys()
	:m_pLockOn(NULL)
{
}

DsGameSys::~DsGameSys()
{
	delete m_pLockOn; m_pLockOn = NULL;
}

void DsGameSys::Initialize()
{
	m_pLockOn = new DsLockOn();
	DS_ASSERT(m_pLockOn, "ÉÅÉÇÉäämï€é∏îs");
}