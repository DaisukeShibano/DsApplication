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
}

namespace DsApp
{
	class DsFieldObj
	{
	public:
		

	public:
		DsFieldObj(DsLib::DsSys& sys, DsPhysics::DsPhysicsWorld& world, DsLib::DsResource& resource);
		virtual ~DsFieldObj();
		bool IsRequestInit() const { return m_reqestIsInit; }
		bool IsCompleteInit() const { return m_isCompleteInit; }

	public:
		virtual void Update(double dt);

	public:
		virtual void Initialize(const DsFieldInitInfo& initInfo);
	protected:
		virtual void _SetActorCoord(DsPhysics::DsActorCoordFactory& factory, const DsFieldInitInfo& initInfo);

	public:
		virtual void SetPosition(const DsVec3d& pos);
		virtual void SetRotation(const DsMat33d& rot);
		virtual DsVec3d GetPosition() const;
		virtual DsMat33d GetRotation() const;

	public:
		DsPhysics::DsActor* GetActor();
		const DsPhysics::DsActor* GetActor() const;

		const DsLib::DsAnimation* GetAnim() const { return m_pAnimation; }
		DsLib::DsAnimation* GetAnim() { return m_pAnimation; }

		void SetRequestAnim(std::string name);

	protected:
		bool IsRequestDirectAnim()const { return m_isRequestDirectAnim; }

	protected:
		DsLib::DsSys& m_sys;
		std::string m_name;
		DsLib::DsResource& m_resource;
		DsLib::DsAnimation* m_pAnimation;
		DsPhysics::DsActorId m_actorId;
		DsPhysics::DsPhysicsWorld& m_world;
		DsAnimEventCallback* m_pAnimEventCallback;
		bool m_reqestIsInit;
		bool m_isCompleteInit;

	private:
		bool m_isRequestDirectAnim;

	public:
		virtual void DbgDraw(DsLib::DsDrawCommand& com);

	};
}


#endif
