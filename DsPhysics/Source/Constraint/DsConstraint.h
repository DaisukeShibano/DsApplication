#ifndef __DS_CONSTRAINT__
#define __DS_CONSTRAINT__

#ifndef __DS_ACTOR__
#include "Actor/DsActor.h"
#endif

#ifndef __ICONSTRAINT__
#include "Constraint/IConstraint.h"
#endif

namespace DsPhysics
{
	class DsPhysicsWorld;
}

namespace DsPhysics
{

	class DsConstraint : public IConstraint
	{

	public:
		DsConstraint(const DsPhysicsWorld& world);
		//                          物体１の位置　　　　　物体２の位置　　　　　                                 拘束の位置                          拘束の基準ベクトル
		DsConstraint(const DsActorId masterId, const DsActorId subId, const DsPhysicsWorld& world, const DsVec3d& pos, const DsVec3d& ex, const DsVec3d& ey, const DsVec3d& ez, const double dt);
		//                          物体１の位置　　　　　物体２の位置　　　　　                                 拘束の位置                          拘束の基準ベクトル
		DsConstraint(const DsActorId masterId, const DsActorId subId, const DsPhysicsWorld& world, const DsVec3d& pos, const DsVec3d& ez, const double dt);
		virtual ~DsConstraint(){}

		virtual void CalclateConstraintForce() override;
		virtual void ApplyConstraintForce() override;
		virtual void SetUp() override;
		virtual DsActorId GetMasterActorId() const override{ return m_masterId; }
		virtual DsActorId GetSubActorId() const override { return m_subId; }
		virtual DsConstraintEquation& RefEq() override { return m_eq; }
		virtual void ClampConstraint(){};

	protected:
		/*
			@param[in] 今の速度
		*/
		virtual void _SetEquation(const double w[6]);

	private:
		void _SetVel(const DsVec3d& mV, const DsVec3d& mW, const DsVec3d& sV, const DsVec3d& sW);
		void _SetMass(const DsActor& master, const DsActor& sub);
		void _SetExForce(const DsVec3d& mF, const DsVec3d& mT, const DsVec3d& sF, const DsVec3d& sT);
		void _CalcConatraintForce();

	protected:
		DsActorId m_masterId;
		DsActorId m_subId;
		const DsPhysicsWorld& m_world;

		DsVec3d m_pos;
		DsVec3d m_ex;
		DsVec3d m_ey;
		DsVec3d m_ez;

		double m_dt;
		DsVec3d m_errorP;

		double m_J[6][12];
		double m_Jt[12][6];

		double m_u[12];
		double m_M[12][12];
		double m_invM[12][12];
		double m_Fe[12];

		DsVec3d m_masterF;
		DsVec3d m_masterT;
		DsVec3d m_subF;
		DsVec3d m_subT;

	protected://LCP用
		DsConstraintEquation m_eq;


		//デバッグ
	public:
		const DsVec3d& GetPosition() const { return m_pos; }
		DsMat33d GetRotation() const 
		{
			return DsMat33d::SetAxis(m_ex, m_ey, m_ez);
		}
	};


}

#endif