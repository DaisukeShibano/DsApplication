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
	private:
		void _UpdateAction();
	public:
		virtual DsVec3d GetMoveVec()const override;
		virtual bool IsAction(ACTION_TYPE type)const override;
		virtual void SetRequest(ACTION_TYPE type) override;
		virtual void SetCancel(ACTION_TYPE type) override;
		virtual void SetCancelAll() override;

	private:
		const DsLib::DsKeyboard& m_key;
		const DsLib::DsMouse& m_mouse;
		DsVec3d m_moveVec;
		double m_moveDir[4];
		ds_uint64 m_request;
		ds_uint64 m_requestPre;
		ds_uint64 m_requestToggle;
		ds_uint64 m_cancel;
		ds_uint64 m_action;

	};

}

#endif