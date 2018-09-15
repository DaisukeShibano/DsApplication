#pragma once

#ifndef _DS_FIELD_INIT_INFO_H_
#include "World/Field/DsFieldInitInfo.h"
#endif

#ifndef _DS_FIELD_OBJ_H_
#include "World/Field/DsFieldObj.h"
#endif

namespace DsLib
{
}

namespace DsPhysics
{
	class DsActorCoordFactory;
}

namespace DsApp
{
	class DsAnimEventCallback;
	class DsComponentSystem;
	class DsActionRequest;
}

namespace DsApp
{
	class DsFieldObstacle : public DsFieldObj
	{
	public:
		DsFieldObstacle(DsLib::DsSys& sys, DsPhysics::DsPhysicsWorld& world);
		virtual ~DsFieldObstacle();

	public:
		virtual void SetPosition(const DsVec3d& pos) override;
		virtual void SetRotation(const DsMat33d& rot) override;
		virtual DsVec3d GetPosition() const override;
		virtual DsMat33d GetRotation() const override;

	public:
		DsPhysics::DsActor* GetActor();
		const DsPhysics::DsActor* GetActor() const;

	protected:
		DsPhysics::DsActorId m_actorId;
	};
}

