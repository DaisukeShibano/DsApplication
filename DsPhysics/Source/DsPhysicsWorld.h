#ifndef __DS_PHYSICS_WORLD__
#define __DS_PHYSICS_WORLD__

#ifndef __DS_COLLISION_GROUP__
#include "Collision/DsCollisionGroup.h"
#endif
#ifndef __DS_ACTOR__
#include "Actor/DsActor.h"
#endif
#ifndef __ICONSTRAINT__
#include "Constraint/IConstraint.h"
#endif

namespace DsPhysics
{
	class DsActorFactory;
	class DsJointFactory;
	class DsJoint;
	class DsCollisionListener;
	class DsConstraintSolver;
	class DsCollisionCallback;
	class DsRay;
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
		typedef std::list<DsActor*> Actors;
		typedef std::list<DsJoint*> Joints;
		typedef std::vector<IConstraint*> ConstraintArray;

	public:
		DsPhysicsWorld( const DsPhysicsWorldSetting setting = DsPhysicsWorldSetting::Default() );
		virtual ~DsPhysicsWorld();

	public:
		DsActorId CreateActor(const DsActorFactory& factory);
		DsActorId DeleteActor(const DsActorId& id);
		DsJoint* CreateJoint(DsJointFactory& jointFactory);
		void DeleteJoint(DsJoint* pJoint);
		void Update( double dt );
		void Clear();
		DsActor* GetActor(const DsActorId& id) const;
		Actors& GetActors() { return m_actors; }
		DsActor* RayCast_CollectNear( const DsVec3d& startPos, const DsVec3d& endPos, double* depth=NULL, DsVec3d* hitPos = NULL) const;
		DsActor* RayCast_CollectNear(DsRay& ray, double* depth = NULL, DsVec3d* hitPos=NULL) const;
		double GetDt() const;
		const DsVec3d& GetGravity() const { m_gravity; }

	public:
		//拘束ソルバーの取得
		DsConstraintSolver* GetConstraintSolver() { return m_pConstraintSolver; }
		const DsConstraintSolver* GetConstraintSolver() const { return m_pConstraintSolver; }

	public:
		//衝突コールバック登録
		void SetCollisionCallback(DsCollisionCallback* pCallback) { m_pCollisionCallback = pCallback; }
		const DsCollisionCallback* GetCollisionCallback() const { return m_pCollisionCallback; }
	
	private:
		void _ApplyGravity();
		void _UpdateJoint(double dt);
		void _UpdateConstraint(double dt);  //毎フレクリアされる。衝突など追加、削除を頻繁に行うと断片化しやすい
		void _UpdateActor();
		void _DeleteNoLifeActor();

	private:
		DsCollisionListener* m_pListener;
		DsCollisionGroup m_group;
		Actors m_actors;
		Joints m_joints;
		DsPhysicsWorldSetting m_setting;
		DsConstraintSolver* m_pConstraintSolver;
		DsCollisionCallback* m_pCollisionCallback;

	private:
		bool m_isGravity;
		DsVec3d m_gravity;

	};
}
#endif