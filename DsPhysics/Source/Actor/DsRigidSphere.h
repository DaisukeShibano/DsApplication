#ifndef __DS_RIGID_SPHERE__
#define __DS_RIGID_SPHERE__

#ifndef __DS_RIGID_BODY__
#include "Actor/DsRigidBody.h"
#endif


namespace DsPhysics
{
	class DsBoundingTreeAabb;
}

namespace DsPhysics
{
	struct DsRigidSphereGeometryInfo
	{
		double r;
	};

	class DsRigidSphere : public DsRigidBody
	{
	public:
		/////////////Factory////////////
	class DsRigidSphereFactory : public DsActorCoordFactory
	{
	public:
		DsRigidSphereFactory(const double r, const double mass, const char* name)
		:DsActorCoordFactory()
		,m_r(r)
		,m_mass(mass)
		,m_initOption(Option::Default())
		,m_name(name)
		{}

		virtual DsActor* CreateIns( const DsActorId& id ) const override;

	public:
		void SetOption(const Option& option){ m_initOption = option; }

	private:
		const double m_r;
		const double m_mass;
		Option m_initOption;
		std::string m_name;
	};
	/////////////Factory‚±‚±‚Ü‚Å////////////


	public:
		DsRigidSphere(const DsActorId& id, const char* name);

		virtual ~DsRigidSphere();

	public:
		virtual DsActor::ACTOR_TYPE GetType() const override{ return DsActor::RIGID_SPHERE; }

	public:	
		void Create( const double r, const double mass );
		
	public:
		virtual void Draw(DsDrawCommand& com) override;
		
	private:
		virtual void _UpdateInertia(const DsMat33d& deltaRot)override;
		virtual void _UpdateAabb()override;

	private:
		DsCollisionGeometry* m_pCollisionGeometry;

	};
}

#endif