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
		DsActionCtrl(DsActionRequest& actReq, const std::vector<DsLib::DsAnimClip*>& animClip);
		virtual ~DsActionCtrl();

	public:
		void Update(double dt);
		DsAnimClip* GetCurrentAnim() const { return m_pCurrentAnim; }

	private:
		DsLib::DsActionStateCtrl* m_pASCtrl;
		DsActionRequest& m_actReq;
		StateMap m_state;
		DsAnimClip* m_pCurrentAnim;
	};


}

#endif