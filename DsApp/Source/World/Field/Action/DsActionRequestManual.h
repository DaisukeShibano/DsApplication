#ifndef _DS_ACTION_REQUEST_MANUAL_
#define _DS_ACTION_REQUEST_MANUAL_

#ifndef _DS_ACTION_REQUEST_
#include "World/Field/Action/DsActionRequest.h"
#endif

namespace DsApp
{
	class DsActionRequestManual : public DsActionRequest
	{
	public:
		DsActionRequestManual(const DsLib::DsSys& sys);
		virtual ~DsActionRequestManual();

	public:
		virtual void Update(double dt) override;
		virtual DsVec3d GetMoveVec()const override;

	private:
		const DsLib::DsKeyboard& m_key;
		DsVec3d m_moveVec;
		double m_moveDir[4];

	};

}

#endif