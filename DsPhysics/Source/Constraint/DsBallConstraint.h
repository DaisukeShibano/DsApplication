#ifndef __DS_BALL_CONSTRAINT__
#define __DS_BALL_CONSTRAINT__

#ifndef __DS_ACTOR__
#include "Actor/DsActor.h"
#endif

#ifndef __DS_CONSTRAINT__
#include "Constraint/DsConstraint.h"
#endif

namespace DsPhysics
{
	class DsPhysicsWorld;
}

namespace DsPhysics
{

	class DsBallConstraint : public DsConstraint
	{

	public:
		DsBallConstraint(const DsPhysicsWorld& world);
		//                          物体１の位置　　　　　物体２の位置　　　　　                                 拘束の位置                          拘束の基準ベクトル
		DsBallConstraint(const DsActorId masterId, const DsActorId subId, const DsPhysicsWorld& world, const DsVec3d& pos, const DsVec3d& ex, const DsVec3d& ey, const DsVec3d& ez, const double dt, const DsVec3d& errorP, const DsMat33d& errorR );
		virtual ~DsBallConstraint(){}

		void RequestAttach(const DsActorId masterId, const DsActorId subId, const DsVec3d& pos, const DsVec3d& ex, const DsVec3d& ey, const DsVec3d& ez, const double dt, const DsVec3d& errorP);

	public:
		virtual void SetUp() override;

	protected:
		virtual void _SetEquation( const double w[6]) override;

	};

}

#endif