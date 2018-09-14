#ifndef __DS_CONSTRAINT_SOLVER__
#define __DS_CONSTRAINT_SOLVER__

#ifndef __DS_COLLISION_RESULT__
#include "Collision/DsCollisionResult.h"
#endif


namespace DsPhysics
{
	class DsPhysicsWorld;
	class IConstraint;
	class DsCollisionConstraint;
	class DsPositionDepthSolver;
	class DsActorId;
	struct DsConstraintEquation;
}

namespace DsPhysics
{
	class DsConstraintSolver
	{
	public:
		DsConstraintSolver(const DsPhysicsWorld& world);
		virtual ~DsConstraintSolver();

	public:
		void AddCollision(const DsCollisionResult& colResult);
		void AddConstraint( IConstraint* pConstraint);
		void RemoveConstraint(IConstraint* pConstraint);
		void Solve(const int maxIteration, double dt);
		
		//idのactorには力をかけず、戻り値として返す。それ以外は拘束力がかかる
		DsVec3d SolveCollision(const DsActorId id, const int maxIteration, double dt);

	public:
		int GetIterationNum() const;
		int GetCurrentIteration() const { return m_currentIteration; }

	private:
		const DsPhysicsWorld& m_world;
		std::list<IConstraint*> m_constraints;
		ds_uint8* m_colConstraintBuff;
		size_t m_colConstraintBuffSize;
		size_t m_colConstraintBuffSizeMin;
		size_t m_colConstraintBuffUseSize;
		double m_colBufMinTimer;

	private:
		std::vector<DsConstraintEquation*> m_eqs;
		int m_currentIteration;
		DsPositionDepthSolver* m_pPosDepthSolver;
	};

}

#endif