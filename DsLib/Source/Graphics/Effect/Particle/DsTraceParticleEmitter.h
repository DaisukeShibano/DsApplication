#ifndef _DS_TRACE_PARTICLE_EMITTER_
#define _DS_TRACE_PARTICLE_EMITTER_

namespace DsLib
{
	struct DsSquareParticle;
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
		void SetPosition(const DsVec3d& pos1, const DsVec3d& pos2);
		void RequestEmit() { m_isRequestEmit = true;}//–ˆƒtƒŒƒNƒŠƒA


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


	};

}



#endif