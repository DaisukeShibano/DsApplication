#ifndef _DS_RIGID_MESH_H_
#define _DS_RIGID_MESH_H_

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

namespace DsLib
{
	class DsAnimModel;
}

namespace DsPhysics
{
	class DsBoundingTreeAabb;
}

namespace DsPhysics
{
	class DsRigidMesh : public DsActor
	{
		public:
		/////////////Factory////////////
		class DsRigidMeshFactory : public DsActorFactory
		{
		public:
			DsRigidMeshFactory(const DsAnimModel& anim, const char* name)
				: m_anim(anim)
				, m_initPos(DsVec3d::Zero())
				, m_initRot(DsMat33d::Identity())
				, m_initOption(Option::Default())
				, m_name(name)
			{}

			virtual DsActor* CreateIns(const DsActorId& id) const override;

		public:
			void InitPos(const DsVec3d& pos){ m_initPos = pos; }
			void InitRot(const DsMat33d& rot){ m_initRot = rot; }
			void SetOption(const Option& option){ m_initOption = option; }

		private:
			const DsAnimModel& m_anim;
			DsVec3d m_initPos;
			DsMat33d m_initRot;
			Option m_initOption;
			std::string m_name;
		};
		/////////////Factoryここまで////////////

		struct DsRigidMeshGeometryInfo
		{
			DsRigidMeshGeometryInfo()
				: pVertex(0)
				, pVertexOriginal(0)
				, pFace(0)
				, pLine(0)
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

		struct DsRigidMeshPhysicsInfo
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

			DsRigidMeshPhysicsInfo()
				: exForce(DsVec3d::Zero())
				, acc(DsVec3d::Zero())
				, vel(DsVec3d::Zero())
				, exTorque(DsVec3d::Zero())
				, angVel(DsVec3d::Zero())
			{}
		};

	public:
		DsRigidMesh(const DsActorId& id, const char* name);
		virtual ~DsRigidMesh();

	public:
		virtual DsActor::ACTOR_TYPE GetType() const override{ return DsActor::RIGID_MESH; }

		virtual const DsCollisionContext* GetCollContext() const override { return m_pCollisionContext; }

		virtual void Update() override;

		virtual const DsVec3d& GetPosition()const override{ return m_physicsInfo.pos; }
		virtual const DsVec3d& GetVelocity()const override{ return m_physicsInfo.vel; }
		virtual const DsVec3d& GetForce()const override{ return m_referenceF; }
		virtual const DsMat33d& GetRotation()const override{ return m_physicsInfo.rotation; }
		virtual const DsVec3d& GetAngularVel()const override{ return m_physicsInfo.angVel; }
		virtual const DsVec3d& GetTorque()const override{ return m_referenceT; }
		virtual const DsMass& GetMass()const override{ return m_physicsInfo.mass; }
		virtual const double* GetMassInv10() const override{ return m_physicsInfo.massInv; }

		virtual void AddForce(const DsVec3d& force)override;
		virtual void AddTorque(const DsVec3d& torque)override;

		virtual const DsActorMaterial& RefMaterial() const override { return m_material; }
		virtual const DsAabb& RefAabb() const override { return m_aabb; }

	public:
		virtual DsVec3d CalcVelocity(const DsVec3d& f) const override;
		virtual DsVec3d CalcAngularVel(const DsVec3d& t) const override;

	public:
		virtual const char* GetName() const override { return m_name.c_str(); }

	public:
		virtual void SetPosition(const DsVec3d& pos){ m_physicsInfo.pos = pos; m_isForceUpdate = true; }
		virtual void SetRotation(const DsMat33d& rot){ m_physicsInfo.rotation = rot; m_isForceRotation = true; }
		virtual void SetVelocity(const DsVec3d& v) override{ m_physicsInfo.vel = v; }
		virtual void SetForce(const DsVec3d& f) override{ m_physicsInfo.exForce = f; }
		virtual void SetMaterial(const DsActorMaterial& material) override { m_material = material; }

	public:
		virtual void SetExVelocity(const DsVec3d& v) override { m_exVel = v; }

	public: //描画のためのメソッド
		virtual void Draw(DsDrawCommand& com) override;

	public:
		void Create(const DsAnimModel& animModel);

	private:
		void _Update(const DsVec3d& deltaPos, const DsMat33d& deltaRot);
		void _UpdateForRest();
		void _IntegralF();
		void _IntegralVel();

	private:
		std::string m_name;
		DsRigidMeshGeometryInfo m_geomInfo;
		DsRigidMeshPhysicsInfo	m_physicsInfo;
		DsActorMaterial m_material;
		bool m_isForceUpdate;
		bool m_isForceRotation;

		DsVec3d m_addPos;
		DsMat33d m_addRot;
		DsVec3d m_initPos;
		DsMat33d m_initRot;
		//参照用の力とトルク。積分するとクリアされてしまうので。
		DsVec3d m_referenceF;
		DsVec3d m_referenceT;
		DsVec3d m_exVel;
		DsAabb m_aabb;
		DsVec3d m_sideSize;
		double m_restTimer;

	private:
		DsBoundingTreeAabb* m_pAabbTree;
		DsCollisionContext* m_pCollisionContext;

	};
}

#endif