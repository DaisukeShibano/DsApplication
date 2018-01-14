#ifndef _DS_ACTION_REQUEST_
#define _DS_ACTION_REQUEST_

namespace DsApp
{
	enum class ACTION_TYPE
	{
	};


	class DsActionRequest
	{
	public:
		DsActionRequest() {};
		virtual ~DsActionRequest() {};

	public:
		virtual void Update(double dt) {};
		virtual DsVec3d GetMoveVec()const { return DsVec3d::Zero(); }
	};

}

#endif