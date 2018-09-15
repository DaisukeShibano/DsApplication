#ifndef _DS_FIELD_OBJ_H_
#define _DS_FIELD_OBJ_H_

#ifndef _DS_FIELD_INIT_INFO_H_
#include "World/Field/DsFieldInitInfo.h"
#endif

namespace DsLib
{
}

namespace DsPhysics
{
	class DsActorCoordFactory;
}

namespace DsApp
{
	class DsAnimEventCallback;
	class DsComponentSystem;
	class DsActionRequest;
}

namespace DsApp
{
	enum class DMYPOLY_SLOT
	{
		MAIN_BODY,
		WEP,
	};


	class DsFieldObj
	{
	public:
		DsFieldObj(DsLib::DsSys& sys, DsPhysics::DsPhysicsWorld& world);
		virtual ~DsFieldObj();
		bool IsRequestInit() const { return m_reqestIsInit; }
		bool IsCompleteInit() const { return m_isCompleteInit; }

	public:
		virtual void Update(double dt);

	public:
		virtual void Initialize(const DsFieldInitInfo& initInfo);

	public:
		virtual DsActionRequest* GetActionRequest() const { return NULL; }

	public:
		virtual void SetPosition(const DsVec3d& pos) = 0;
		virtual void SetRotation(const DsMat33d& rot) = 0;
		virtual DsVec3d GetPosition() const = 0;
		virtual DsMat33d GetRotation() const = 0;

	public:
		const DsLib::DsAnimation* GetAnim() const { return m_pAnimation; }
		DsLib::DsAnimation* GetAnim() { return m_pAnimation; }
		bool GetDmypoly(int id, std::vector<DsMat44d>& outMat, DMYPOLY_SLOT slot)const;
		bool GetDmypoly(int id, DsMat44d& outMat, DMYPOLY_SLOT slot)const;

		void SetRequestAnim(std::string name);

	public:
		DsComponentSystem* GetComponentSystem() const { return m_pComponentSystem; }

	protected:
		bool IsRequestDirectAnim()const { return m_isRequestDirectAnim; }

	protected:
		DsLib::DsSys& m_sys;
		std::string m_name;
		DsLib::DsAnimation* m_pAnimation;
		DsPhysics::DsPhysicsWorld& m_world;
		DsAnimEventCallback* m_pAnimEventCallback;
		DsComponentSystem* m_pComponentSystem;
		bool m_reqestIsInit;
		bool m_isCompleteInit;
	private:
		bool m_isRequestDirectAnim;



	public:
		virtual void DbgDraw(DsLib::DsDrawCommand& com) {};

	};
}


#endif
