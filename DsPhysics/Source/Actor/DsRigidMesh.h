#ifndef _DS_RIGID_MESH_H_
#define _DS_RIGID_MESH_H_

#ifndef __DS_RIGID_BODY__
#include "Actor/DsRigidBody.h"
#endif

namespace DsLib
{
	class DsModel;
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
		class DsRigidMeshFactory : public DsActorCoordFactory
		{
		public:
			DsRigidMeshFactory(const DsModel& anim, const char* name)
				: DsActorCoordFactory()
				, m_anim(anim)
				, m_initOption(Option::Default())
				, m_name(name)
			{}

			virtual DsActor* CreateIns(const DsActorId& id) const override;

		public:
			void SetOption(const Option& option){ m_initOption = option; }

		private:
			const DsModel& m_anim;
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
		void Create(const DsModel& animModel);

	private:
		virtual void _Update(const DsVec3d& deltaPos, const DsMat33d& deltaRot);

	private:
		DsBoundingTreeAabb* m_pAabbTree;

	};
}

#endif