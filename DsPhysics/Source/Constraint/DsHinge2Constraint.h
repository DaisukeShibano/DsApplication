#ifndef __DS_HINGE2_CONSTRAINT__
#define __DS_HINGE2_CONSTRAINT__

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
	class DsHinge2Constraint : public DsBallConstraint
	{

	public:
		/*
		YZŽ²‚ª‰ñ“]Ž²‚Èƒqƒ“ƒW
		*/
		DsHinge2Constraint(const DsActorId masterId, const DsActorId subId, const DsPhysicsWorld& world, const DsVec3d& posE, const DsVec3d& ex, const DsVec3d& ey, const DsVec3d& ez, const double dt, const DsVec3d& errorP, const DsMat33d& errorR);
		DsHinge2Constraint(const DsPhysicsWorld& world);
		virtual ~DsHinge2Constraint(){}

	public:
		virtual void SetUp() override;
		
	public:
		void UpdateErrorAxis1(const DsVec3d& axisMas1, const DsVec3d& axisSub1)
		{
			m_axisMas1 = axisMas1;
			m_axisSub1 = axisSub1;
		}
		void UpdateErrorAxis2(const DsVec3d& axisMas2, const DsVec3d& axisSub2)
		{
			m_axisMas2 = axisMas2;
			m_axisSub2 = axisSub2;
		}

	protected:
		virtual void _SetEquation( const double b[6]) override;

	private:
		DsVec3d m_axisMas1;
		DsVec3d m_axisSub1;
		DsVec3d m_axisMas2;
		DsVec3d m_axisSub2;
		double m_lambda[6];

	};
}

#endif