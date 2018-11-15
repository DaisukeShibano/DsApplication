#ifndef _DS_TRACE_PARTICLE_EMITTER_
#define _DS_TRACE_PARTICLE_EMITTER_

#ifndef _DS_PARTICLE_
#include "Graphics/Effect/Particle/DsParticle.h"
#endif
#include "Graphics/Effect/Particle/DsParticleEmitter.h"


namespace DsLib
{
}

namespace DsLib
{
	class DsTraceParticleEmitter : public DsParticleEmitter
	{
	public:
		DsTraceParticleEmitter();
		virtual ~DsTraceParticleEmitter();

	public:
		void Update(double dt);
		void RequestEmit(const DsVec3d& pos1, const DsVec3d& pos2);
		virtual double GetParticleMaxLifeTime()const override;
		bool IsEmpty()const;

	public:
		virtual void EnumParticle(const EnumType& func) const override;

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
		double m_nextEmitTime;
		bool m_isRequestEmit;
		bool m_isRequestEmitPre;
	};

}



#endif