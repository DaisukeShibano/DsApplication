#ifndef _DS_ACTION_REQUEST_
#define _DS_ACTION_REQUEST_

namespace DsApp
{
	enum class ACTION_TYPE
	{
		IDLE,
		MOVE,
	};


	class DsActionRequest
	{
	public:
		DsActionRequest(const DsLib::DsSys& sys);
		virtual ~DsActionRequest();

	public:
		void Update(double dt);
		DsVec3d GetMoveVec()const;
		ACTION_TYPE GetAction() const;

	private:
		const DsLib::DsKeyboard& m_key;
		DsVec3d m_moveVec;
		double m_moveDir[4];

	};

}

#endif