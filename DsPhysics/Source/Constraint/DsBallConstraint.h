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

#if 0
	class DsBallConstraint
	{
	public:
		class ConstraintType
		{
		public:
			virtual bool Solve( double dst[6], const double curW[6], const double A[6][6], const double b[6] ) const  = 0;
		};

	public:
		class Collision : public ConstraintType
		{
		public:
			Collision(const double e ):m_e(e){}
			bool Solve( double dst[6], const double curW[6], const double A[6][6], const double b[6] ) const override;
		private:
			double m_e;
		};



	public:
		//                 物体１の位置　　　　　物体２の位置　　　　　拘束の位置　　　　　　　　　拘束の基準ベクトル　　
		DsBallConstraint(const DsActor& master, const DsActor& sub, const DsVec3d& posE, const DsVec3d& ex, const DsVec3d& ey, const DsVec3d& ez, const double dt);
		virtual ~DsBallConstraint(){}

		void CalcConatraintForce( DsVec3d& masterF, DsVec3d& masterT, DsVec3d& subF, DsVec3d& subT, const ConstraintType& constraint );
		
	private:
		void _SetVel( const DsActor& master, const DsActor& sub );
		void _SetMass( const DsActor& master, const DsActor& sub );
		void _SetExForce( const DsActor& master, const DsActor& sub );

	public:
		const double m_dt;
		double m_J[6][12];
		double m_Jt[12][6];
		double m_u[12];
		double m_M[12][12];
		double m_invM[12][12];
		double m_Fe[12];
	};
#else
	class DsBallConstraint : public DsConstraint
	{

	public:
		DsBallConstraint(const DsPhysicsWorld& world);
		//                          物体１の位置　　　　　物体２の位置　　　　　                                 拘束の位置                          拘束の基準ベクトル
		DsBallConstraint(const DsActorId masterId, const DsActorId subId, const DsPhysicsWorld& world, const DsVec3d& pos, const DsVec3d& ex, const DsVec3d& ey, const DsVec3d& ez, const double dt, const DsVec3d& errorP, const DsMat33d& errorR );
		virtual ~DsBallConstraint(){}

		void RequestAttach(const DsActorId masterId, const DsActorId subId, const DsVec3d& pos, const DsVec3d& ex, const DsVec3d& ey, const DsVec3d& ez, const double dt, const DsVec3d& errorP);

	protected:
		virtual void _SetEquation( const double w[6]) override;

	};


#endif



}

#endif