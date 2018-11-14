#pragma once
#ifndef _DS_PARTICLE_
#include "Graphics/Effect/Particle/DsParticle.h"
#endif

namespace DsLib
{
}

namespace DsLib
{
	class DsBloodParticleEmitter
	{
	public:
		DsBloodParticleEmitter();
		virtual ~DsBloodParticleEmitter();

	public:
		void Update(double dt);
		void RequestEmit(const DsVec3d& pos, const DsVec3d& dir);
		std::string GetTexPath() const { return m_texPath; }
		double GetParticleMaxLifeTime()const;
		bool IsEmpty()const;

	public:
		template<typename FUNC>
		void EnumParticle(FUNC func) const{
			
		}

	private:
		std::vector<DsSquareParticle> m_particle;
		DsVec3d m_reqPos;
		DsVec3d m_reqDir;
		std::string m_texPath;
		double m_lineTime;
		bool m_isRequest;
	};

}
