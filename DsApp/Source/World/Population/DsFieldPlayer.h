#ifndef _DS_FIELD_PLAYER_H_
#define _DS_FIELD_PLAYER_H_

#ifndef _DS_FIELD_CHR_H_
#include "World/Population/DsFieldChr.h"
#endif

namespace DsPhysics
{
	class DsRagdoll;
}

namespace DsApp
{
	class DsAnimRagdollModifier;
}

namespace DsApp
{
	class DsFieldPlayer : public DsFieldChr
	{
	public:
		DsFieldPlayer(DsLib::DsSys& sys, DsPhysics::DsPhysicsWorld& world);
		virtual ~DsFieldPlayer();

	public:
		virtual void Initialize(const DsFieldInitInfo& initInfo);
		virtual void Update(double dt);

	private:
		void _UpdateCam(double dt);

	protected:
		virtual DsLib::DsAnimController& RefAnimController() override;

	private:
		DsLib::DsCamera& m_cam;
		DsLib::DsMouse& m_mouse;
		DsLib::DsWindow& m_window;
		DsLib::DsActReq m_actReq;
		DsPhysics::DsRagdoll* m_pRagdoll;
		DsAnimRagdollModifier* m_pAnimRagdollModifier;

	public:
		virtual void DbgDraw(DsLib::DsDrawCommand& com);

	};
}


#endif
