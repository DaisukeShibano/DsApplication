#pragma once


namespace DsApp
{
	class DsGameSys;
	class DsFieldObjectCreator;
	class DsAppCollisionCallback;
}

namespace DsApp
{
	class DsGameWorld
	{
	public:
		DsGameWorld(DsLib::DsSys& sys);
		virtual~DsGameWorld();

	public:
		void Initialize();
		void Update(double dt);

	private:
		DsLib::DsSys& m_sys;
		DsPhysics::DsPhysicsWorld* m_pPhysWorld;
		DsGameSys* m_pGameSys;
		DsFieldObjectCreator* m_pFieldObjectCreator;
		DsAppCollisionCallback* m_pCollisionCallback;
	};
}