#ifndef _DS_FIELD_PLAYER_H_
#define _DS_FIELD_PLAYER_H_

#ifndef _DS_FIELD_CHR_H_
#include "World/Population/DsFieldChr.h"
#endif

namespace DsLib
{
	class DsHitRes;
}

namespace DsApp
{
	class DsFieldPlayer : public DsFieldChr
	{
	public:
		DsFieldPlayer(DsLib::DsSys& sys, DsPhysics::DsPhysicsWorld& world);
		virtual ~DsFieldPlayer();

	public:
		virtual void Initialize(const InitInfo& initInfo);
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


	public:
		virtual void DbgDraw(DsLib::DsDrawCommand& com);

	};
}


#endif
