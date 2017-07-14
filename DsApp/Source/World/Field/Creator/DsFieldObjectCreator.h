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
	class DsFieldHit;
	class DsFieldObj;
	class DsFieldChr;
}

namespace DsApp
{
	class DsFieldObjectCreator
	{
	private:
		struct _InitInfo
		{
			DsFieldInitInfo initInfo;
			DsFieldObj* pObj;
		};

	public:
		DsFieldObjectCreator(DsLib::DsSys& sys);
		virtual ~DsFieldObjectCreator();

	public:
		void Create(const char* resPath, DsLib::DsResource& res, DsPhysics::DsPhysicsWorld& world);
		void Update(double dt);
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

		std::vector<_InitInfo> m_requestHits;
		std::vector<_InitInfo> m_requestObjs;
		std::vector<_InitInfo> m_requestChrs;
	};
}

#endif