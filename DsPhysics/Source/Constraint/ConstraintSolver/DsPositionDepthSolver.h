#ifndef _DS_POSITION_DEPTH_SOLVER_
#define _DS_POSITION_DEPTH_SOLVER_

namespace DsPhysics
{
	class DsCollisionResult;
	class DsPhysicsWorld;
}

namespace DsPhysics
{
	class DsPositionDepthSolver
	{
	public:
		DsPositionDepthSolver(const DsPhysicsWorld& world);
		~DsPositionDepthSolver();

	public:
		void AddDepth(const DsCollisionResult& c);

	private:
		const DsPhysicsWorld& m_world;
	};

	
}

#endif