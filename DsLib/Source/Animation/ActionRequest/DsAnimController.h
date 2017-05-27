#ifndef _DS_ANIM_CONTROLLER_H_
#define _DS_ANIM_CONTROLLER_H_

namespace DsLib
{
	class DsAnimController
	{
	public:
		static DsAnimController& Null()
		{
			static DsAnimController ret;
			return ret;
		}

	public:
		DsAnimController();
		virtual	~DsAnimController();

	public:
		virtual void Update(double dt);
		virtual bool IsMove() const;
	};
}

#endif