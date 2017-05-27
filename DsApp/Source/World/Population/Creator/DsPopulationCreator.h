#ifndef _DS_POPULATION_CREATOR_H_
#define _DS_POPULATION_CREATOR_H_

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
	class DsFieldHit;
	class DsFieldObj;
	class DsFieldChr;
}

namespace DsApp
{
	class DsPopulationCreator
	{
	public:
		DsPopulationCreator(DsLib::DsSys& sys);
		virtual ~DsPopulationCreator();

	public:
		void Create(const char* resPath, DsLib::DsResource& res, DsPhysics::DsPhysicsWorld& world);
		void Destoroy();

	public:
		std::vector<DsFieldHit*> RefHits() { return m_hits; }
		std::vector<DsFieldObj*> RefObjs() { return m_objs; }
		std::vector<DsFieldChr*> RefChrs() { return m_chrs; }

	private:
		DsLib::DsSys& m_sys;
		std::vector<DsFieldHit*> m_hits;
		std::vector<DsFieldObj*> m_objs;
		std::vector<DsFieldChr*> m_chrs;
	};
}

#endif