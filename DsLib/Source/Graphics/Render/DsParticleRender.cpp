#include "DsPch.h"

#include <gl/GL.h>
#include <gl/GLU.h>

#ifndef _DS_PARTICLE_RENDER_
#include "Graphics/Render/DsParticleRender.h"
#endif
//他のヘッダ
#include "Graphics/Effect/Particle/DsParticleEmitter.h"
#ifndef _DS_CAMERA_H_
#include "Graphics/Camera/DsCamera.h"
#endif

using namespace DsLib;


DsParticleRender::DsParticleRender(const DsCamera& cam)
	: m_emitterList()
	, m_texture()
	, m_cam(cam)
{
}

DsParticleRender::~DsParticleRender()
{
}

void DsParticleRender::Register(const DsParticleEmitter& emitter)
{
	m_emitterList.push_back(&emitter);
	m_texture.Load(emitter.GetTexPath());
}

void DsParticleRender::UnRegister(const DsParticleEmitter& emitter)
{
	m_texture.UnLoad(emitter.GetTexPath());
	m_emitterList.remove(&emitter);
}

void DsParticleRender::Render() const
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);

	const DsVec3d camDir = -m_cam.GetRot().GetAxisZ();

	for (const DsParticleEmitter* pEmitter : m_emitterList) {
		const double maxLifeTime = pEmitter->GetParticleMaxLifeTime();
		const GLuint texId = m_texture.GetTexId(pEmitter->GetTexPath());
		glBindTexture(GL_TEXTURE_2D, texId);
		//材質の色は適当
		const GLfloat diff[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		const GLfloat spec[] = { 0.0f, 0.0f, 0.0f, 1.0f };	// 鏡面反射色
		const GLfloat ambi[] = { 0.1f, 0.1f, 0.1f, 1.0f };	// 環境光
		glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambi);
		glMaterialf(GL_FRONT, GL_SHININESS, 30);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diff);

		//パーティクルループ
		{
			auto func = [this, texId, camDir, maxLifeTime](const DsSquareParticle& particle) {
				const DsVec2f* pUv = particle.uvPos;
				const DsVec3d* pPos = particle.pos;
				const GLfloat col[] = { 1.0f, 1.0f, 1.0f, static_cast<float>(particle.lifeTime/ maxLifeTime) };
				
				glColor4fv(col);
				glNormal3dv(camDir.v);
				glBegin(GL_QUADS);
				glTexCoord2fv(pUv[0].v);
				glVertex3dv(pPos[0].v);
				glTexCoord2fv(pUv[1].v);
				glVertex3dv(pPos[1].v);
				glTexCoord2fv(pUv[2].v);
				glVertex3dv(pPos[2].v);
				glTexCoord2fv(pUv[3].v);
				glVertex3dv(pPos[3].v);
				glEnd();
			};
			pEmitter->EnumParticle(func);
		}
	}

	glEnable(GL_CULL_FACE);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);

}