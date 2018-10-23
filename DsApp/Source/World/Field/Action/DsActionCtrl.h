#ifndef _DS_ACTION_CTRL_
#define _DS_ACTION_CTRL_

#include "World/Field/Action/DsChrState.h"

namespace DsApp
{
	class DsActionRequest;
	class DsAnimEventFlags;
}

namespace DsApp
{
	class DsActionCtrl
	{
	public:
		DsActionCtrl(DsActionRequest& actReq, const DsAnimEventFlags& animFlags, const std::vector<DsLib::DsAnimClip*>& animClip);
		virtual ~DsActionCtrl();

	public:
		void Update(double dt);
		DsAnimClip* GetCurrentAnim() const { return m_pCurrentAnim; }

	private:
		DsLib::DsActionStateCtrl* m_pASCtrl;
		StateMap m_state;
		DsAnimClip* m_pCurrentAnim;
	};


}

#endif