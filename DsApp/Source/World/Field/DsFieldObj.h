#ifndef _DS_FIELD_OBJ_H_
#define _DS_FIELD_OBJ_H_

#ifndef _DS_FIELD_INIT_INFO_H_
#include "World/Field/DsFieldInitInfo.h"
#endif

namespace DsLib
{
}

namespace DsApp
{
	class DsFieldObj
	{
	public:
		

	public:
		DsFieldObj(DsLib::DsSys& sys, DsPhysics::DsPhysicsWorld& world);
		virtual ~DsFieldObj();
		bool IsRequestInit() const { return m_reqestIsInit; }
		bool IsCompleteInit() const { return m_isCompleteInit; }

	public:
		virtual void Update(double dt);
		virtual void Initialize(const DsFieldInitInfo& initInfo);
		virtual void SetPosition(const DsVec3d& pos);
		virtual void SetRotation(const DsMat33d& rot);
		virtual DsVec3d GetPosition() const;
		virtual DsMat33d GetRotation() const;

	public:
		DsPhysics::DsActor* GetActor();
		const DsPhysics::DsActor* GetActor() const;

	protected:
		DsLib::DsSys& m_sys;
		std::string m_name;
		DsLib::DsAnimation* m_pAnimation;
		DsPhysics::DsActorId m_actorId;
		DsPhysics::DsPhysicsWorld& m_world;
		bool m_reqestIsInit;
		bool m_isCompleteInit;

	public:
		virtual void DbgDraw(DsLib::DsDrawCommand& com);

	};
}


#endif
