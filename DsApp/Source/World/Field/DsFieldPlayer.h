#ifndef _DS_FIELD_PLAYER_H_
#define _DS_FIELD_PLAYER_H_

#ifndef _DS_FIELD_CHR_H_
#include "World/Field/DsFieldChr.h"
#endif

namespace DsApp
{

}

namespace DsApp
{
	/*
	プレイヤーは１人しか存在しない想定
	*/
	class DsFieldPlayer : public DsFieldChr
	{
	public:
		DsFieldPlayer(DsLib::DsSys& sys, DsPhysics::DsPhysicsWorld& world);
		virtual ~DsFieldPlayer();

	public:
		virtual void Initialize(const DsFieldInitInfo& initInfo) override;
	protected:
		virtual DsActionRequest* _CreareActionRequest() override;

	public:
		virtual void Update(double dt);

	private:
		void _UpdateCam(double dt);

	private:
		DsLib::DsCamera& m_cam;
		DsLib::DsMouse& m_mouse;
		DsLib::DsWindow& m_window;
		DsGameSys* m_pGameSys;
		bool m_isLockOn;
		bool m_isReqLockOn;

	public:
		virtual void DbgDraw(DsLib::DsDrawCommand& com);
	};
}


#endif
