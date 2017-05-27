#ifndef _DS_LIGHT_MAN_H_
#define _DS_LIGHT_MAN_H_

namespace DsLib
{
	class DsLight;
}

namespace DsLib
{
	class DsLightMan
	{
	public:
		DsLightMan();
		virtual ~DsLightMan();

	public:
		static DsLightMan& GetIns()
		{
			static DsLightMan ret;
			return ret;
		}

	public:
		DsLight& GetLight(unsigned int lightNo);
		//‘Î‰žŒõ‚ÌŽæ“¾
		DsLight& GetSunLight();
		void Enable();
		void Disable();

	public:
		void Update();

	};
}

#endif