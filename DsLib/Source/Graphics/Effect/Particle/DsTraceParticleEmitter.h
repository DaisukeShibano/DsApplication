#ifndef _DS_TRACE_PARTICLE_EMITTER_
#define _DS_TRACE_PARTICLE_EMITTER_

#ifndef _DS_PARTICLE_
#include "Graphics/Effect/Particle/DsParticle.h"
#endif

namespace DsLib
{
}

namespace DsLib
{
	class DsTraceParticleEmitter
	{
	public:
		DsTraceParticleEmitter();
		virtual ~DsTraceParticleEmitter();

	public:
		void Update(double dt);
		void RequestEmit(const DsVec3d& pos1, const DsVec3d& pos2);
		std::string GetTexPath() const { return m_texPath; }
		double GetParticleMaxLifeTime()const;
		bool IsEmpty()const;

	public:
		template<typename FUNC>
		void EnumParticle(FUNC func) const{
			for (const ParticleBlock& block : m_particle) {
				const DsSquareParticle* pParticle = block.pUseHead;
				const DsSquareParticle* pPre = NULL;
				while (pParticle) {
					func(*pParticle);
					pParticle = pParticle->pNext;
				}
			}
		}

	private:
		struct ParticleBlock
		{
			DsSquareParticle* pDataTop;
			DsSquareParticle* pUseHead;
			DsSquareParticle* pUseTail;
			DsSquareParticle* pFreeTail;
			double deleteBlockTime;
		};

	private:
		std::list<ParticleBlock> m_particle;
		DsVec3d m_emitPos[2];
		DsVec3d m_emitPrePos[2];
		std::string m_texPath;
		double m_nextEmitTime;
		bool m_isRequestEmit;
		bool m_isRequestEmitPre;
	};

}



#endif