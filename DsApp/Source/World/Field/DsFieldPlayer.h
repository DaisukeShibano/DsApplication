#ifndef _DS_FIELD_PLAYER_H_
#define _DS_FIELD_PLAYER_H_

#ifndef _DS_FIELD_CHR_H_
#include "World/Field/DsFieldChr.h"
#endif


namespace DsApp
{
	class DsFieldPlayer : public DsFieldChr
	{
	public:
		DsFieldPlayer(DsLib::DsSys& sys, DsPhysics::DsPhysicsWorld& world);
		virtual ~DsFieldPlayer();

	public:
		virtual void Initialize(const DsFieldInitInfo& initInfo);
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

	public:
		virtual void DbgDraw(DsLib::DsDrawCommand& com);

	};
}


#endif
