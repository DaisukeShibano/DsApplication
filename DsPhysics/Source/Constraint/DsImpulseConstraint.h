#ifndef _DS_IMPULSE_CONSTRAINT_H_
#define _DS_IMPULSE_CONSTRAINT_H_


#ifndef __ICONSTRAINT__
#include "Constraint/IConstraint.h"
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
	class DsImpulseConstraint : public IConstraint
	{

	public:
		//                          物体１の位置　　　　　物体２の位置　　　　　                                 拘束の位置　　　　　　拘束の基準ベクトル　　
		DsImpulseConstraint(const DsActorId master, const DsActorId sub, const DsPhysicsWorld& world, const DsVec3d& colPos, const DsVec3d& colNormal, const double dt, const double error);
		virtual ~DsImpulseConstraint(){}

		virtual void CalclateConstraintForce(int iteNum) override;
		virtual void ApplyConstraintForce() override{};
		virtual DsActorId GetMasterActorId() const override{ return m_masterId; }
		virtual DsActorId GetSubActorId() const override { return m_subId; }

	private:
		void _SetUp();
		void _CalcConatraintForce(DsVec3d& masterF, DsVec3d& masterT, DsVec3d& subF, DsVec3d& subT);

	private:
		const DsActorId m_masterId;
		const DsActorId m_subId;
		const DsPhysicsWorld& m_world;

		const DsVec3d m_colPos;
		const DsVec3d m_colNormal;

		const double m_dt;
		const double m_error;
		double m_e1;
		double m_e2;

		DsVec3d m_colVel;
		DsVec3d m_fricVel;
		DsVec3d m_colMasVec;
		DsVec3d m_colSubVec;
		DsVec3d m_exMasF;
		DsVec3d m_exSubF;
		
	};
}


#endif