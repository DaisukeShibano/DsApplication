#ifndef __DS_COLLISION_CONSTRAINT__
#define __DS_COLLISION_CONSTRAINT__

#ifndef __DS_CONSTRAINT__
#include "Constraint/DsConstraint.h"
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
	//衝突全体の拘束を一度に解くための情報
	//w[t+1] = Aλ + b 
	struct DsCollisionConstraintEquation
	{
#ifdef DS_SYS_USE_SIMD_
		double A[3][4];
#else
		double A[3][3];
#endif
		double lambda[3];
		double b[3];
		double min_wplus1[3];
		double max_wplus1[3];
		double fricLambda[2];
		bool isStaticByFric[2];
	};

	class DsCollisionConstraint : public IConstraint
	{
	
	public:
		//                          物体１の位置　　　　　物体２の位置　　　　　                                 拘束の位置　　　　　　拘束の基準ベクトル　　
		DsCollisionConstraint(const DsActorId master, const DsActorId sub, const DsPhysicsWorld* world, const DsVec3d& colPos, const DsVec3d& colNormal, const double dt, const double error)
			: m_masterId(master)
			, m_subId(sub)
			, m_world(world)
			, m_colPos(colPos)
			, m_colNormal(colNormal)
			, m_dt(dt)
			, m_error(error)
			, m_bound(0.0)
			, m_sFric(0.0)
			, m_kFric(0.0)
			, m_isSetUp(false)
			, m_cEq()
		{}
		void Initialize(const DsActorId master, const DsActorId sub, const DsPhysicsWorld* world, const DsVec3d& colPos, const DsVec3d& colNormal, const double dt, const double error)
		{
			m_masterId = master;
			m_subId = sub;
			m_world = world;
			m_colPos = colPos;
			m_colNormal = colNormal;
			m_dt = dt;
			m_error = error;
			m_bound = 0.0;
			m_sFric = 0.0;
			m_kFric = 0.0;
			m_isSetUp = false;
		}
		
		DsCollisionConstraint(){}
		virtual ~DsCollisionConstraint(){}
	
		DsCollisionConstraintEquation& RefEquation() { return m_cEq; }

	public:
		virtual void CalclateConstraintForce() override;
		virtual void ApplyConstraintForce() override;
		virtual void SetUp() override;
		virtual DsActorId GetMasterActorId() const override{ return m_masterId; }
		virtual DsActorId GetSubActorId() const override { return m_subId; }
		virtual DsConstraintEquation& RefEq(){ static DsConstraintEquation dummy(this); return dummy; }
		double GetDt() const { return m_dt; }

	private:
		void _SetVel(const DsVec3d& mV, const DsVec3d& mW, const DsVec3d& sV, const DsVec3d& sW);
		void _SetMass( const DsActor& master, const DsActor& sub );
		void _SetExForce(const DsVec3d& mF, const DsVec3d& mT, const DsVec3d& sF, const DsVec3d& sT);
		void _CalcConatraintForce();

	private:
		DsActorId m_masterId;
		DsActorId m_subId;
		const DsPhysicsWorld* m_world;

		DsVec3d m_colPos;
		DsVec3d m_colNormal;

		double m_dt;
		double m_error;
		double m_bound;
		double m_sFric;
		double m_kFric;

		double m_J[3][12];
#ifdef DS_SYS_USE_SIMD_
		double m_Jt[12][4];
#else
		double m_Jt[12][3];
#endif

		double m_u[12];
		double m_M[12][12];
		double m_invM[12][12];
		double m_Fe[12];

		double m_w[3];

		bool m_isSetUp;

		DsCollisionConstraintEquation m_cEq;

		//デバッグ
	private:
		DsVec3d m_dbgConstraintBaseCoord[4];
	};

	//拘束全体を一気に解く
	class DsCollisionConstraintSolver
	{
	public:
		DsCollisionConstraintSolver(){};
		virtual ~DsCollisionConstraintSolver(){};

	public:
		void Solve(DsCollisionConstraint& c, const int iterationNum);
	};	
}

#endif