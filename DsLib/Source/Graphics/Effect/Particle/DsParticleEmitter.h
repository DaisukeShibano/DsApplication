#pragma once
#ifndef _DS_PARTICLE_
#include "Graphics/Effect/Particle/DsParticle.h"
#endif
#include <functional>


/*
#include "Graphics/Effect/Particle/DsParticleEmitter.h"
*/

namespace DsLib
{
}

namespace DsLib
{
	class DsParticleEmitter
	{
	public:
		typedef std::function<void(const DsSquareParticle&)> EnumType;

	public:
		DsParticleEmitter()
			: m_albedoTexPath()
			, m_normalTexPath()
			, m_isUseNormalMap(false)
		{}

	public:
		std::string GetAlbedoTexPath() const { return m_albedoTexPath; }
		std::string GetNormalTexPath() const { return m_normalTexPath; }
		bool IsUseNormalMap() const { return m_isUseNormalMap; }
		virtual double GetAlpha(const double lifeTime)const = 0;

	public:
		virtual void EnumParticle(const EnumType& func) const = 0;

	protected:
		std::string m_albedoTexPath;
		std::string m_normalTexPath;
		bool m_isUseNormalMap;
	};

}
