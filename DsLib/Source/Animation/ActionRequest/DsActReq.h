#ifndef _DS_ACT_REQ_H_
#define _DS_ACT_REQ_H_

#ifndef _DS_ANIM_CONTROLLER_H_
#include "Animation/ActionRequest/DsAnimController.h"
#endif

namespace DsLib
{
	class DsSys;
	class DsKeyboard;
}

namespace DsLib
{
	class DsActReq : public DsAnimController
	{
	public:
		enum
		{
			//ƒAƒNƒVƒ‡ƒ“
		};

	public:
		DsActReq(const DsSys& sys);
		virtual ~DsActReq();

	public:
		virtual void Update(double dt) override;
		virtual bool IsMove() const override;
		const DsVec3d& GetMoveVec() const { return m_moveVec; }

	protected:
		const DsKeyboard& m_key;
		DsVec3d m_moveVec;
		double m_moveDir[4];
	};
}

#endif