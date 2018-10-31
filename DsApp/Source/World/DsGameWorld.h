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
		DsGameWorld();
		virtual~DsGameWorld();

	public:
		void Initialize(DsLib::DsSys& sys);
		void Update(double dt);

	private:
		DsPhysics::DsPhysicsWorld* m_pPhysWorld;
		DsGameSys* m_pGameSys;
		DsFieldObjectCreator* m_pFieldObjectCreator;
		DsAppCollisionCallback* m_pCollisionCallback;
	};
}