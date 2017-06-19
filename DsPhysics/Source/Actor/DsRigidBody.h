#ifndef __DS_RIGID_BODY__
#define __DS_RIGID_BODY__

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
#ifndef __DS_COLLISION_GEOMETRY__
#include "Collision/DsCollisionGeometry.h"
#endif
#ifndef __DS_ACTOR_FACTORY__
#include "Actor/DsActorFactory.h"
#endif


namespace DsPhysics
{
	struct DsRigidGeometryInfo
	{
			DsRigidGeometryInfo()
			: pVertex(NULL)
			, pVertexOriginal(NULL)
			, pFace(NULL)
			, pLine(NULL)
			, vn(0)
			, fn(0)
			, ln(0)
		{

		}
		DsVec3d* pVertex;
		DsVec3d* pVertexOriginal;//姿勢変化する前の頂点
		DsQuad*	pFace;
		DsLine*	pLine;

		int vn;
		int fn;
		int ln;
	};

	struct DsRigidPhysicsInfo
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

		DsRigidPhysicsInfo()
			:exForce(DsVec3d::Zero())
			, acc(DsVec3d::Zero())
			, vel(DsVec3d::Zero())
			, exTorque(DsVec3d::Zero())
			, angVel(DsVec3d::Zero())
		{}
	};

	class DsRigidBody : public DsActor
	{
	public:
		DsRigidBody(const DsActorId& id, const char* name);

	public:
		virtual void Update();

		virtual const DsCollisionGeometry* GetCollisionGeometry() const override { return m_pCollisionGeometry; }

		virtual const DsVec3d& GetPosition()const override { return m_physicsInfo.pos; }
		virtual const DsVec3d& GetVelocity()const override { return m_physicsInfo.vel; }
		virtual const DsVec3d& GetForce()const override { return m_physicsInfo.exForce; }
		virtual const DsMat33d& GetRotation()const override { return m_physicsInfo.rotation; }
		virtual const DsVec3d& GetAngularVel()const override { return m_physicsInfo.angVel; }
		virtual const DsVec3d& GetTorque()const override { return m_physicsInfo.exTorque; }
		virtual const DsMass& GetMass()const override { return m_physicsInfo.mass; }
		virtual const double* GetMassInv10() const override { return m_physicsInfo.massInv; }

		virtual void AddForce(const DsVec3d& force)override;
		virtual void AddTorque(const DsVec3d& torque)override;
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
		virtual void SetPosition(const DsVec3d& pos) override { m_physicsInfo.pos = pos; m_isForceUpdate = true; }
		virtual void SetRotation(const DsMat33d& rot) override { m_physicsInfo.rotation = rot; m_isForceRotation = true; }
		virtual void SetVelocity(const DsVec3d& v) override { m_physicsInfo.vel = v; }
		virtual void SetAngularVelocity(const DsVec3d& v) override { m_physicsInfo.angVel = v; }
		virtual void SetForce(const DsVec3d& f) override { m_physicsInfo.exForce = f; m_referenceF = f; }
		virtual void SetMaterial(const DsActorMaterial& material) override { m_material = material; }
		virtual void AddVelocity(const DsVec3d& v) override { m_physicsInfo.vel += v; }
		virtual void AddAngularVelocity(const DsVec3d& v) override { m_physicsInfo.angVel += v; }
		virtual void SetDamper(double vel, double angVel) override { m_damperVel = vel; m_damperAngVel = angVel; }
		virtual void SetInertiaBias(const DsVec3d& bias) override { m_biasInertia = bias; }

	protected:
		DsVec3d _GetMaxVector(const DsVec3d* pVec, const int vn) const;
		DsVec3d _GetCenterOfGravity(const DsVec3d* v, const int vertexNum)const;
		

	protected:
		virtual void _Update(const DsVec3d& deltaPos, const DsMat33d& deltaRot);
		virtual void _UpdateInertia(const DsMat33d& deltaRot);
		virtual void _UpdateAabb(bool isChangeRot);
		virtual void _UpdateForRest();
		virtual void _IntegralF();
		virtual void _IntegralVel();

	protected:
		DsRigidGeometryInfo m_geomInfo;
		DsRigidPhysicsInfo	m_physicsInfo;
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
		DsAabb m_aabb;
		DsVec3d m_sideSize;
		double m_restTimer;
		DsVec3d m_prePos;
		DsMat33d m_preRot;
		double m_damperVel;
		double m_damperAngVel;
		DsVec3d m_biasInertia;

	protected:
		DsCollisionGeometry* m_pCollisionGeometry;

	};
}

#endif