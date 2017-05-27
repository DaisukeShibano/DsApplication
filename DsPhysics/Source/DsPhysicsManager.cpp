#include "DsPhysicsPch.h"
#include "DsPhysicsManager.h"
#include "DsPhysicsWorld.h"

using namespace DsPhysics;

static 	DsPhysicsWorld* m_pDefaultWorld=0;



bool DsPhysicsManager::SetUpDefault()
{
	if(0 == m_pDefaultWorld)
	{
		m_pDefaultWorld = new DsPhysicsWorld();
		if (m_pDefaultWorld)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		printf("Šù‚ÉSetUpDefault()‚ÍŽÀsÏ‚Ý‚Å‚·B");
		return true;
	}
}

DsPhysicsWorld* DsPhysicsManager::GetDefaultWorld()
{
	return m_pDefaultWorld;
}


void DsPhysicsManager::ShutDown()
{
	if(m_pDefaultWorld)
	{
		m_pDefaultWorld->Clear();
		delete m_pDefaultWorld;
		m_pDefaultWorld=0;
	}
}