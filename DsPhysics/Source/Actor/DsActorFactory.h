#ifndef __DS_ACTOR_FACTORY__
#define __DS_ACTOR_FACTORY__

namespace DsPhysics
{
	class DsActor;
	class DsActorId;
}

namespace DsPhysics
{
	class DsActorFactory
	{
	public:
		virtual DsActor* CreateIns( const DsActorId& id ) const = 0;
	};

	class DsActorCoordFactory : public DsActorFactory
	{
	public:
		DsActorCoordFactory()
			: m_initPos()
			, m_initRot(DsMat33d::Identity())
		{}
	public:
		virtual DsActor* CreateIns(const DsActorId& id) const override = 0;
		void InitPos(const DsVec3d& pos) { m_initPos = pos; }
		void InitRot(const DsMat33d& rot) { m_initRot = rot; }

	protected:
		DsVec3d m_initPos;
		DsMat33d m_initRot;
	};

}
#endif