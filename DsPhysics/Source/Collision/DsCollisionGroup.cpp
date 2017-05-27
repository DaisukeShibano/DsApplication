#include "DsPhysicsPch.h"
#include "DsCollisionGroup.h"
#include "Actor/DsActor.h"

#include <iostream>
#include <assert.h>
using namespace DsPhysics;

DsCollisionGroup::DsCollisionGroup()
	:m_totalActorNum(0)
{
	Clear();
}

DsCollisionGroup::~DsCollisionGroup()
{
	Clear();
}


void DsCollisionGroup::AddGroup( DsActor* pActor )
{
	if( NULL == pActor)return;

	//最大値チェック
	if( MAX_ACTOR <= m_totalActorNum )return;

	//重複チェック
	for(int actIdx=0; m_totalActorNum > actIdx; ++actIdx)
	{
		//インスタンス重複
		if( pActor == m_actorVec[actIdx] )return;

		//Id重複
		if(  m_actorVec[actIdx] )
		{	
			if( pActor->GetId() == m_actorVec[actIdx]->GetId() )return;
		}
	}

	DS_ASSERT( (NULL == m_actorVec[m_totalActorNum]), "重複チェック漏れ");
	m_actorVec[m_totalActorNum] = pActor;
	++m_totalActorNum;
}

void DsCollisionGroup::RemoveGroup( const DsActorId& id )
{
	for(int actIdx=0; MAX_ACTOR > actIdx; ++actIdx)
	{
		if( NULL != m_actorVec[actIdx] )
		{
			if( m_actorVec[actIdx]->GetId() == id )
			{
				m_actorVec[actIdx] = NULL;
				--m_totalActorNum;
				_ReArrangement();
				return;
			}
		}
	}
}

void DsCollisionGroup::SortActor()
{
	class Prd{
	public:
		bool operator()(const DsActor* l, const DsActor* r) const {
			return l->GetPosition().y < r->GetPosition().y;
		}
	};
	std::sort(m_actorVec, m_actorVec + (m_totalActorNum - 1), Prd());
}

void DsCollisionGroup::Clear()
{
	for(int actIdx=0; MAX_ACTOR > actIdx; ++actIdx)
	{
		m_actorVec[actIdx] = NULL;
	}
}

void DsCollisionGroup::_ReArrangement()
{
	int notNullNum = (0 != m_actorVec[0]) ? 1 : 0;

	for(int actIdx=1; MAX_ACTOR > actIdx; ++actIdx)
	{
		if( 0 != m_actorVec[actIdx] )
		{
			if( 0 == m_actorVec[actIdx-1] )
			{
				DsActor* temp = m_actorVec[notNullNum];
				m_actorVec[notNullNum] = m_actorVec[actIdx];
				m_actorVec[actIdx] = temp;
				
			}
			++notNullNum;
		}
	}
}
