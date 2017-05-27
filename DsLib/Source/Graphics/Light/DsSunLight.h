#ifndef _DS_SUNLIGHT_H_
#define _DS_SUNLIGHT_H_

#ifndef _DS_LIGHT_H_
#include "Graphics/Light/DsLight.h"
#endif

namespace DsLib
{
	class DsSunLight : public DsLight
	{
	public:
		DsSunLight(unsigned int lightNo);
		virtual ~DsSunLight();

	public:
		virtual void SetPos(const DsVec3f& pos) override;
		virtual void SetDir(const DsVec3f& dir) override;

	public:
		void Update();
	};
}

#endif