#pragma once

#ifndef _DS_COMPONENT_
#include "World/Component/DsComponent.h"
#endif

namespace DsApp
{
	class DsHitEffectComponent : public DsComponent
	{
	public:
		DsHitEffectComponent();
		virtual ~DsHitEffectComponent();

	public:
		virtual bool Update(COMPONENT_UPDATE_RESULT& outResult, const COMPONENT_UPDATE_ARG& arg) override;

	public:
		void Request(int effectId, const DsVec3d& hitPos, DsVec3d hitDir);

	private:
		bool m_isRequest;
		int m_effectId;
		DsVec3d m_hitPos;
		DsVec3d m_hitDir;
		DsLib::DsSys* m_pSys;
		DsLib::DsBloodParticleEmitter* m_pEmitter;
	};
}
