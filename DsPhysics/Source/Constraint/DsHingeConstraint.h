#ifndef __DS_HINGE_CONSTRAINT__
#define __DS_HINGE_CONSTRAINT__

#ifndef __DS_BALL_CONSTRAINT__
#include "Constraint/DsBallConstraint.h"
#endif

#ifndef __DS_ACTOR__
#include "Actor/DsActor.h"
#endif


namespace DsPhysics
{
	class DsPhysicsWorld;
}

namespace DsPhysics
{
	class DsHingeConstraint : public DsBallConstraint
	{

	public:
		//                 物体１の位置　　　　　物体２の位置　　　　　拘束の位置　　　　　　　　                                            　拘束の基準ベクトル　　
		DsHingeConstraint(const DsActorId masterId, const DsActorId subId, const DsPhysicsWorld& world, const DsVec3d& posE, const DsVec3d& ex, const DsVec3d& ey, const DsVec3d& ez, const double dt, const DsVec3d& errorP, const DsMat33d& errorR);
		DsHingeConstraint(const DsPhysicsWorld& world);
		virtual ~DsHingeConstraint(){}

	public:
		void SetAxis( const DsVec3d& maserAxis, const DsVec3d& subAxis);

	public:
		virtual void SetUp() override;
		virtual void ClampConstraint() override;
		virtual void ApplyConstraintForce() override;

	protected:
		virtual void _SetEquation( const double b[6]) override;

	private:
		DsVec3d m_axisMas;
		DsVec3d m_axisSub;
		double m_lambda[6];

	};
}

#endif