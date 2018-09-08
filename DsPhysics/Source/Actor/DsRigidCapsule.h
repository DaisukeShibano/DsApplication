#pragma once

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
	class DsRigidCapsuleFactory : public DsActorCoordFactory
	{
	public:
		DsRigidCapsuleFactory( const double r, const double halfLen, const double mass, const char* name )
		:DsActorCoordFactory()
		,m_r(r)
		,m_halfLen(halfLen)
		,m_mass(mass)
		,m_initOption(Option::Default())
		,m_name(name)
		,m_biasI(DsVec3d::Zero())
		{}

		virtual DsActor* CreateIns( const DsActorId& id ) const override;

		DsActor* CreateIns(const DsActorId& id, void* pBuffer) const;

	public:
		void SetOption(const Option& option){ m_initOption = option; }
		void SetBiasI(const DsVec3d& bias) { m_biasI = bias; }

	private:
		const double m_r;
		const double m_halfLen;
		const double m_mass;
		Option m_initOption;
		std::string m_name;
		DsVec3d m_biasI;
	};
	/////////////Factoryここまで////////////


	public:
		DsRigidCapsule(const DsActorId& id, const char* name);

		virtual ~DsRigidCapsule();

	public:
		virtual DsActor::ACTOR_TYPE GetType() const override{ return DsActor::RIGID_CAPSULE; }


	public:	
		//vertexは引数を内部で持ってるメンバにコピー
		void Create(double r, double halfLen, double mass );

		//カプセルの伸びてる方向のデフォ値
		static DsVec3d GetInitExtendDir();

	private:
		virtual void _UpdateAabb() override;

	public: //描画のためのメソッド
		virtual void Draw(DsDrawCommand& com) override;
	};
}
