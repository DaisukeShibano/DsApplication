#ifndef _DS_ACTION_CTRL_
#define _DS_ACTION_CTRL_

#ifndef _DS_CHR_STATE_
#include "World/Field/Action/DsChrState.h"
#endif

namespace DsApp
{
	class DsChrState;
	class DsActionRequest;
}

namespace DsApp
{
	class DsActionCtrl
	{
	public:
		DsActionCtrl(const DsLib::DsSys& sys);
		virtual ~DsActionCtrl();

	public:
		void Update(double dt);

	private:
		DsLib::DsActionStateCtrl* m_pASCtrl;
		DsActionRequest* m_pActReq;
		StateMap m_state;

	};


}

#endif