#ifndef _DS_JOINT_
#define _DS_JOINT_


namespace DsPhysics
{
	class DsPhysicsWorld;
}

namespace DsPhysics
{
	class DsJoint
	{
	public:
		virtual void Update(double dt) = 0;

		DsJoint( DsPhysicsWorld& world )
		: m_world(world)
		{}
		virtual ~DsJoint(){}
		DsPhysicsWorld& m_world;


	public:
		virtual void DbgDraw(DsDrawCommand& com) const{};


	};

	class DsJointFactory
	{
	public:
		virtual DsJoint* CreateIns()=0;
	};
}


#endif