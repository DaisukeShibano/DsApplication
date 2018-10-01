#pragma once



#ifndef _DS_ACTION_REQUEST_
#include "World/Field/Action/DsActionRequest.h"
#endif

namespace DsApp
{
	class DsActionRequestCom1 : public DsActionRequest
	{
	public:
		DsActionRequestCom1(const DsLib::DsSys& sys);
		virtual ~DsActionRequestCom1();

	public:
		virtual void Update(double dt) override;
	public:
		virtual DsVec3d GetMoveVec()const override;
		virtual bool IsAction(ACTION_TYPE type)const override;
		virtual bool IsMove()const override;
		virtual void SetRequest(ACTION_TYPE type) override;
		virtual void SetCancel(int type) override;
		virtual void SetCancelAll() override;
		virtual void SetInput(int type) override;

	private:
		DsVec3d m_moveVec;
		ds_uint64 m_request;
	};

}