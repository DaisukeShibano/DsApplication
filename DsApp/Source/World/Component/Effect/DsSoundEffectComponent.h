#ifndef _DS_SOUND_EFFECT_COMPONENT_
#define _DS_SOUND_EFFECT_COMPONENT_

#ifndef _DS_COMPONENT_
#include "World/Component/DsComponent.h"
#endif

namespace DsApp
{
	class DsSoundEffectComponent : public DsComponent
	{
	public:
		DsSoundEffectComponent();
		virtual ~DsSoundEffectComponent();

	public:
		virtual bool Update(COMPONENT_UPDATE_RESULT& outResult, const COMPONENT_UPDATE_ARG& arg) override;
		void Request(int soundId, int dmypolyId);

	private:
		int m_soundId;
		int m_dmypolyId;
	};
}

#endif
