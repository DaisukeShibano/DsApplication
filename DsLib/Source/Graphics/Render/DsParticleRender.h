#ifndef _DS_PARTICLE_RENDER_
#define _DS_PARTICLE_RENDER_

#ifndef _DS_TEXTURE_H_
#include "Graphics/Render/DsTexture.h"
#endif

namespace DsLib
{
	class DsParticleEmitter;
	class DsCamera;
	class DsShader;
}

namespace DsLib
{
	class DsParticleRender
	{
	public:
		DsParticleRender(const DsCamera& cam);
		~DsParticleRender();

		void Initialize(DsShader& shader);

	public:
		void Register(const DsParticleEmitter& emitter);
		void UnRegister(const DsParticleEmitter& emitter);

	public:
		void Render() const;

	private:
		std::list<const DsParticleEmitter*> m_emitterList;
		DsTexture m_texture;
		const DsCamera& m_cam;
		DsShader* m_pShader;
	};

}



#endif