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
			, m_specularTexPath()
			, m_isUseNormalMap(false)
			, m_isUseSpecularMap(false)
		{}

	public:
		std::string GetAlbedoTexPath() const { return m_albedoTexPath; }
		std::string GetNormalTexPath() const { return m_normalTexPath; }
		std::string GetSpecularTexPath() const { return m_specularTexPath; }
		bool IsUseNormalMap() const { return m_isUseNormalMap; }
		bool IsUseSpecularMap() const { return m_isUseSpecularMap; }
		virtual double GetAlpha(const double lifeTime)const = 0;

	public:
		virtual void EnumParticle(const EnumType& func) const = 0;

	protected:
		std::string m_albedoTexPath;
		std::string m_normalTexPath;
		std::string m_specularTexPath;
		bool m_isUseNormalMap;
		bool m_isUseSpecularMap;
	};

}
