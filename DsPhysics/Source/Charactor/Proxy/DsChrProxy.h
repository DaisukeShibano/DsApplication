#ifndef _DS_CHR_PROXY_H_
#define _DS_CHR_PROXY_H_

#ifndef __DS_ACTOR__
#include "Actor/DsActor.h"
#endif

namespace DsPhysics
{
}

namespace DsPhysics
{
	class DsChrProxy
	{
	public:
		DsChrProxy();
		virtual ~DsChrProxy();

	public:
		void Update(double dt);
		void SetVel(const DsVec3d& vel){ m_vel = vel; }
		//y軸はグローバル。x軸はy軸回転後
		void SetAng(double x, double y){ m_ang = DsVec3d::Get(x,y,0); }

		const DsVec3d& GetAng() const { return m_ang; }
		const DsVec3d GetPos() const;
		const DsVec3d GetVel() const;
		const DsMat33d& GetRot() const;

	private:
		DsActorId m_actorId;
		DsVec3d m_vel;
		DsVec3d m_ang;
	};
}

#endif