#ifndef _DS_ACTION_REQUEST_
#define _DS_ACTION_REQUEST_

/*
#ifndef _DS_ACTION_REQUEST_
#include "World/Field/Action/DsActionRequest.h"
#endif
*/

namespace DsApp
{
	enum class ACTION_TYPE
	{
		NONE,
		ATTACK,
		CHANGE_WEP,
		MOVE,
		NUM,
	};


	class DsActionRequest
	{
	public:
		DsActionRequest() {};
		virtual ~DsActionRequest() {};

	public:
		virtual void Update(double dt) {};
		virtual DsVec3d GetMoveVec()const { return DsVec3d::Zero(); }
		virtual bool IsAction(ACTION_TYPE type)const { return false; }
		virtual bool IsMove()const { return false; }
		virtual void SetRequest(ACTION_TYPE type){};
		virtual void SetCancel(ACTION_TYPE type){};
		virtual void SetCancelAll() {};
	};

}

#endif