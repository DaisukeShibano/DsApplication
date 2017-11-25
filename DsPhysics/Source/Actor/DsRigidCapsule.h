#ifndef __DS_RIGID_CAPSULE__
#define __DS_RIGID_CAPSULE__

#ifndef __DS_RIGID_BODY__
#include "Actor/DsRigidBody.h"
#endif



namespace DsPhysics
{
}

namespace DsPhysics
{
	class DsRigidCapsule : public DsRigidBody
	{
	public:
		/////////////Factory////////////
	class DsRigidCapsuleFactory : public DsActorFactory
	{
	public:
		DsRigidCapsuleFactory( const double r, const double halfLen, const double mass, const char* name )
		:m_r(r)
		,m_halfLen(halfLen)
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
		const double m_halfLen;
		const double m_mass;
		DsVec3d m_initPos;
		DsMat33d m_initRot;
		Option m_initOption;
		std::string m_name;
	};
	/////////////Factoryここまで////////////


	public:
		DsRigidCapsule(const DsActorId& id, const char* name);

		virtual ~DsRigidCapsule();

	public:
		virtual DsActor::ACTOR_TYPE GetType() const override{ return DsActor::RIGID_CAPSULE; }


	public:	
		//vertexは引数を内部で持ってるメンバにコピー
		void Create(const double r, const double halfLen, const double mass );
		virtual void _UpdateAabb() override;

	public: //描画のためのメソッド
		virtual void Draw(DsDrawCommand& com) override;
		


	private:
		DsCollisionGeometry* m_pCollisionGeometry;

	};
}

#endif