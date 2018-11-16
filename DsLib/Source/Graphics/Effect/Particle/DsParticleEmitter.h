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
			:m_texPath()
		{}

	public:
		std::string GetTexPath() const { return m_texPath; }
		virtual double GetAlpha(const double lifeTime)const = 0;

	public:
		virtual void EnumParticle(const EnumType& func) const = 0;

	protected:
		std::string m_texPath;
	};

}
