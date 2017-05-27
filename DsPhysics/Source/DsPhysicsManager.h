#ifndef __DS_PHYSICS_MANAGER__
#define __DS_PHYSICS_MANAGER__

namespace DsPhysics
{
	class DsPhysicsWorld;
}

namespace DsPhysics
{
	class DsPhysicsManager
	{
	
	public:
		static bool SetUpDefault();

		static void ShutDown();

		static DsPhysicsWorld* GetDefaultWorld();

	private:
		DsPhysicsManager(){}
		virtual ~DsPhysicsManager(){}
	};

};

#endif