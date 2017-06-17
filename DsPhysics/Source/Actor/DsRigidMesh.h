#ifndef _DS_RIGID_MESH_H_
#define _DS_RIGID_MESH_H_

#ifndef __DS_RIGID_BODY__
#include "Actor/DsRigidBody.h"
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
	class DsRigidMesh : public DsRigidBody
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
		/////////////FactoryÇ±Ç±Ç‹Ç≈////////////

	public:
		DsRigidMesh(const DsActorId& id, const char* name);
		virtual ~DsRigidMesh();

	public:
		virtual DsActor::ACTOR_TYPE GetType() const override{ return DsActor::RIGID_MESH; }

	public: //ï`âÊÇÃÇΩÇﬂÇÃÉÅÉ\ÉbÉh
		virtual void Draw(DsDrawCommand& com) override;

	public:
		void Create(const DsAnimModel& animModel);

	private:
		DsBoundingTreeAabb* m_pAabbTree;

	};
}

#endif