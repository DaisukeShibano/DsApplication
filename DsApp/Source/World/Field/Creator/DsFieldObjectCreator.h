#ifndef _DS_FIELD_OBJECT_CREATOR_H_
#define _DS_FIELD_OBJECT_CREATOR_H_

#ifndef _DS_FIELD_INIT_INFO_H_
#include "World/Field/DsFieldInitInfo.h"
#endif

namespace DsLib
{
	class DsResource;
}

namespace DsPhysics
{
	class DsPhysicsWorld;
}

namespace DsApp
{
	class DsFieldObj;
	class DsFieldHit;
	class DsFieldObstacle;
	class DsFieldChr;
}

namespace DsApp
{
	class DsFieldObjectCreator
	{
	private:
		struct INIT_INFO
		{
			DsFieldInitInfo initInfo;
			DsFieldObj* pObj;
		};

	public:
		DsFieldObjectCreator(DsLib::DsSys& sys);
		virtual ~DsFieldObjectCreator();

	public:
		void Create(const char* resPath, DsPhysics::DsPhysicsWorld& world);
		void Update(double dt);
		void Destoroy();

	public:
		std::vector<DsFieldHit*> RefHits() { return m_hits; }
		std::vector<DsFieldObstacle*> RefObstacles() { return m_obstacles; }
		std::vector<DsFieldChr*> RefChrs() { return m_chrs; }

	private:
		DsLib::DsSys& m_sys;
		std::vector<DsFieldHit*> m_hits;
		std::vector<DsFieldObstacle*> m_obstacles;
		std::vector<DsFieldChr*> m_chrs;

		std::vector<INIT_INFO> m_requestHits;
		std::vector<INIT_INFO> m_requestObstacles;
		std::vector<INIT_INFO> m_requestChrs;
	};
}

#endif