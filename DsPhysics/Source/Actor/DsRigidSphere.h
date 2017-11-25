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
	class DsRigidSphereFactory : public DsActorFactory
	{
	public:
		DsRigidSphereFactory(const double r, const double mass, const char* name)
		:m_r(r)
		,m_mass(mass)
		,m_initPos(DsVec3d::Zero())
		,m_initRot(DsMat33d::Identity())
		,m_initOption(Option::Default())
		,m_name(name)
		{}

		virtual DsActor* CreateIns( const DsActorId& id ) const override;

	public:
		void InitPos(const DsVec3d& pos){m_initPos = pos;}
		void InitRot(const DsMat33d& rot){m_initRot = rot;}
		void SetOption(const Option& option){ m_initOption = option; }

	private:
		const double m_r;
		const double m_mass;
		DsVec3d m_initPos;
		DsMat33d m_initRot;
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