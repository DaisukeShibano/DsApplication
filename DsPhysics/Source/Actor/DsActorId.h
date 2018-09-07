#pragma once

/*コピペ用
#include "Actor/DsActorId.h"
*/

namespace DsPhysics
{
	class DsActor;
}

namespace DsPhysics
{
	class DsActorId
	{
		friend DsActor;

	public:
		DsActorId()
			:m_pActor(NULL)
		{}
		DsActorId(DsActor* pActor) :m_pActor(pActor) {}
		bool IsValid()const { return (NULL != m_pActor); }
		bool operator ==(const DsActorId& id)const { return (m_pActor == id.m_pActor); }
		bool operator !=(const DsActorId& id)const { return (m_pActor != id.m_pActor); }
		const DsActor* GetActor() const { return m_pActor; }
		DsActor* GetActor() { return m_pActor; }

	private:
		void _SetActor(DsActor* actor) { m_pActor = actor; }

	private:
		DsActor * m_pActor;
	};
}