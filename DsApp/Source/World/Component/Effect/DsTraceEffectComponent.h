#ifndef _DS_TRACE_EFFECT_COMPONENT_
#define _DS_TARCE_EFFECT_COMPONENT_

#ifndef _DS_COMPONENT_
#include "World/Component/DsComponent.h"
#endif


namespace DsApp
{
	class DsTraceEffectComponent : public DsComponent
	{
	public:
		DsTraceEffectComponent();
		virtual ~DsTraceEffectComponent();

	public:
		void Request(int effectId, int dmypolyId0, int dmypolyId1);

	public:
		virtual bool Update(COMPONENT_UPDATE_RESULT& outResult, const COMPONENT_UPDATE_ARG& arg) override;

	private:
		DsLib::DsTraceParticleEmitter* m_pEmitter;
		DsLib::DsSys* m_pSys;
		int m_effectId;
		int m_dmypolyId0;
		int m_dmypolyId1;
		bool m_isRequest;
	};



}

#endif