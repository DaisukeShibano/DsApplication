#ifndef __DS_PHYSICS_WORLD__
#define __DS_PHYSICS_WORLD__

#ifndef __DS_COLLISION_LISTENER__
#include "Collision/DsCollisionListener.h"
#endif 
#ifndef __DS_COLLISION_GROUP__
#include "Collision/DsCollisionGroup.h"
#endif
#ifndef __DS_ACTOR__
#include "Actor/DsActor.h"
#endif
#ifndef __ICONSTRAINT__
#include "Constraint/IConstraint.h"
#endif
#ifndef __DS_CONSTRAINT_SOLVER__
#include "Constraint/ConstraintSolver/DsConstraintSolver.h"
#endif

namespace DsPhysics
{
	class DsActorFactory;
}

namespace DsPhysics
{
	class DsPhysicsWorldSetting
	{
	public:
		static DsPhysicsWorldSetting Default();

		double GetGravity() const { return m_gravity; }
		double GetDT() const { return m_dt; }
		void SetDT(double dt){ m_dt = dt; }

	private:
		DsPhysicsWorldSetting(){};

	private:
		double m_gravity;
		double m_dt;
	};

	class DsPhysicsWorld
	{
	public:
		typedef std::list<DsActor*> ActorArray;
		typedef std::vector<IConstraint*> ConstraintArray;

	public:
		DsPhysicsWorld( const DsPhysicsWorldSetting setting = DsPhysicsWorldSetting::Default() );
		virtual ~DsPhysicsWorld();

	public:
		DsActorId CreateActor(const DsActorFactory& factory);
		DsActorId DeleteActor(const DsActorId& id);
		void Update( double dt );
		void Clear();
		DsActor* GetActor(const DsActorId& id) const;
		ActorArray& GetActors() { return m_actors; }
		DsActor* RayCast_CollectNear( const DsVec3d& startPos, const DsVec3d& endPos, double* depth=NULL ) const;
		double GetDt() const;
		const DsVec3d& GetGravity() const { m_gravity; }

	public:
		//拘束ソルバーの取得
		DsConstraintSolver& RefConstraintSolver(){ return m_constraintSolver; }
		const DsConstraintSolver& RefConstraintSolver() const { return m_constraintSolver; }

	
	private:
		void _ApplyGravity();
		void _UpdateConstraint(double dt);  //毎フレクリアされる。衝突など追加、削除を頻繁に行うと断片化しやすい
		void _UpdateActor();
		void _DeleteNoLifeActor();

	private:
		DsCollisionListener m_listener;
		DsCollisionGroup m_group;
		ActorArray m_actors;
		DsPhysicsWorldSetting m_setting;
		DsConstraintSolver m_constraintSolver;

	private:
		bool m_isGravity;
		DsVec3d m_gravity;

	};
}
#endif