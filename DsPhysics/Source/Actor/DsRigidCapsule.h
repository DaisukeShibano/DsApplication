#ifndef __DS_RIGID_CAPSULE__
#define __DS_RIGID_CAPSULE__

#ifndef __DS_ACTOR__
#include "Actor/DsActor.h"
#endif
#ifndef __DS_MASS__
#include "DsMass.h"
#endif
#ifndef __DS_FACE__
#include "DsFace.h"
#endif
#ifndef __DS_LINE__
#include "DsLine.h"
#endif
#ifndef __DS_COLLISION_CONTEXT__
#include "Collision/DsCollisionContext.h"
#endif
#ifndef __DS_ACTOR_FACTORY__
#include "Actor/DsActorFactory.h"
#endif


namespace DsPhysics
{
	class DsBoundingTreeAabb;
}

namespace DsPhysics
{
	struct DsRigidCapsuleGeometryInfo
	{
		double r;
		double halfLen;
	};

	struct DsRigidCapsulePhysicsInfo
	{
		DsMass	mass;
		double massInv[10];
		DsMat33d rotation;
		DsVec3d exForce;
		DsVec3d acc;
		DsVec3d vel;
		DsVec3d pos;
		DsVec3d exTorque;
		DsVec3d angVel;
		DsVec3d centerOfGravity;

		DsRigidCapsulePhysicsInfo()
		:exForce(DsVec3d::Zero())
		,acc(DsVec3d::Zero())
		,vel(DsVec3d::Zero())
		,exTorque(DsVec3d::Zero())
		,angVel(DsVec3d::Zero())
		{}
	};

	class DsRigidCapsule : public DsActor
	{
	public:
		/////////////Factory////////////
	class DsRigidCapsuleFactory : public DsActorFactory
	{
	public:
		DsRigidCapsuleFactory( const double r, const double halfLen, const double mass, const char* name )
		:m_r(r)
		,m_halfLen(halfLen)
		,m_mass(mass)
		,m_initPos(DsVec3d::Zero())
		,m_initRot(DsMat33d::Identity())
		,m_initOption(Option::Default())
		,m_name(name)
		{}

		virtual DsActor* CreateIns( const DsActorId& id ) const override;

	public:
		void InitPos(const DsVec3d& pos){m_initPos = pos;}
		void InitRot(const DsMat33d& rot){m_initRot = rot;}
		void SetOption(const Option& option){ m_initOption = option; }

	private:
		const double m_r;
		const double m_halfLen;
		const double m_mass;
		DsVec3d m_initPos;
		DsMat33d m_initRot;
		Option m_initOption;
		std::string m_name;
	};
	/////////////Factoryここまで////////////


	public:
		DsRigidCapsule(const DsActorId& id, const char* name);

		virtual ~DsRigidCapsule();

	public:
		virtual DsActor::ACTOR_TYPE GetType() const override{ return DsActor::RIGID_CAPSULE; }

		virtual const DsCollisionContext* GetCollContext() const override { return m_pCollisionContext; }

		virtual void Update() override;

		virtual const DsVec3d& GetPosition()const override{ return m_physicsInfo.pos; }
		virtual const DsVec3d& GetVelocity()const override{ return m_physicsInfo.vel; }
		virtual const DsVec3d& GetForce()const override{ return m_physicsInfo.exForce; }
		virtual const DsMat33d& GetRotation()const override{ return m_physicsInfo.rotation; }
		virtual const DsVec3d& GetAngularVel()const override{ return m_physicsInfo.angVel; }
		virtual const DsVec3d& GetTorque()const override{ return m_physicsInfo.exTorque; }
		virtual const DsMass& GetMass()const override{ return m_physicsInfo.mass; }
		virtual const double* GetMassInv10() const override{ return m_physicsInfo.massInv; }

		virtual void AddForce(const DsVec3d& force )override;
		virtual void AddTorque(const DsVec3d& torque )override;
		virtual void AddConstraintForce(const DsVec3d& f, const DsVec3d& t) override;
		virtual void IntegrateConstraintForce() override;

		virtual const DsActorMaterial& RefMaterial() const override { return m_material; }
		virtual const DsAabb& RefAabb() const override { return m_aabb; }
	public:
		virtual DsVec3d CalcVelocity(const DsVec3d& f) const override;

		virtual DsVec3d CalcAngularVel(const DsVec3d& t) const override;

	public:
		virtual const char* GetName() const override { return m_name.c_str(); }

	public:
		virtual void SetPosition(const DsVec3d& pos) override{ m_physicsInfo.pos = pos; m_isForceUpdate = true; }
		virtual void SetRotation(const DsMat33d& rot) override { m_physicsInfo.rotation = rot; m_isForceRotation = true; }
		virtual void SetVelocity(const DsVec3d& v) override{ m_physicsInfo.vel = v; }
		virtual void SetAngularVelocity(const DsVec3d& v) override{ m_physicsInfo.angVel = v; }
		virtual void SetForce(const DsVec3d& f) override{ m_physicsInfo.exForce = f; }
		virtual void SetMaterial(const DsActorMaterial& material) override { m_material = material; }
		virtual void AddVelocity(const DsVec3d& v) override { m_physicsInfo.vel += v; }
		virtual void AddAngularVelocity(const DsVec3d& v) override { m_physicsInfo.angVel += v; }


	public:
		virtual void SetExVelocity(const DsVec3d& v) override { m_exVel = v; }

	public:	
		//vertexは引数を内部で持ってるメンバにコピー
		void Create(const double r, const double halfLen, const double mass );
		
	public: //描画のためのメソッド
		virtual void Draw(DsDrawCommand& com) override;
		

	public:
		//寸法からvertexを得る
		static void GetVertex( DsVec3d* pv, double xl, double yl, double zl);
		

	private:
		void _Update( const DsVec3d& deltaPos, const DsMat33d& deltaRot );
		void _UpdateForRest();
		void _IntegralF();
		void _IntegralVel();

	private:
		DsRigidCapsuleGeometryInfo m_geomInfo;
		DsRigidCapsulePhysicsInfo	m_physicsInfo;
		bool m_IsInit;
		DsVec3d m_addPos;
		DsMat33d m_addRot;
		DsVec3d m_initPos;
		DsMat33d m_initRot;
		DsActorMaterial m_material;
		bool m_isForceUpdate;
		bool m_isForceRotation;
		std::string m_name;
		//参照用の力とトルク。積分するとクリアされてしまうので。
		DsVec3d m_referenceF;
		DsVec3d m_referenceT;
		DsVec3d m_constraintF;
		DsVec3d m_constraintT;
		DsVec3d m_exVel;
		DsAabb m_aabb;
		DsVec3d m_sideSize;
		double m_restTimer;

	private:
		DsCollisionContext* m_pCollisionContext;

	};
}

#endif