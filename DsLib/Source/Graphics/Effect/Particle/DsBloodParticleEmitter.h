#pragma once
#ifndef _DS_PARTICLE_
#include "Graphics/Effect/Particle/DsParticle.h"
#endif
#include "Graphics/Effect/Particle/DsParticleEmitter.h"

namespace DsLib
{
}

namespace DsLib
{
	class DsBloodParticleEmitter : public DsParticleEmitter
	{
	public:
		DsBloodParticleEmitter();
		virtual ~DsBloodParticleEmitter();

	public:
		void Update(double dt);
		void RequestEmit(const DsVec3d& pos, const DsVec3d& dir);
		virtual double GetAlpha(const double lifeTime)const override;
		bool IsEmpty()const;

	public:
		virtual void EnumParticle(const EnumType& func) const override;

	private:
		std::vector<DsSquareParticle> m_particle;
		DsVec3d m_reqPos;
		DsVec3d m_reqDir;
		double m_lifeTime;
		bool m_isRequest;
	};

}
