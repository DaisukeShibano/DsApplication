#include "DsPch.h"

#include <gl/GL.h>
#include <gl/GLU.h>

#ifndef _DS_PARTICLE_RENDER_
#include "Graphics/Render/DsParticleRender.h"
#endif
//���̃w�b�_
#ifndef _DS_TRACE_PARTICLE_EMITTER_
#include "Graphics/Effect/Particle/DsTraceParticleEmitter.h"
#endif
#ifndef _DS_CAMERA_H_
#include "Graphics/Camera/DsCamera.h"
#endif

using namespace DsLib;


DsParticleRender::DsParticleRender(const DsCamera& cam)
	: m_traceDrawList()
	, m_texture()
	, m_cam(cam)
{
}

DsParticleRender::~DsParticleRender()
{
}

void DsParticleRender::Register(const DsTraceParticleEmitter& emitter)
{
	m_traceDrawList.push_back(&emitter);
	m_texture.Load(emitter.GetTexPath());
}

void DsParticleRender::UnRegister(const DsTraceParticleEmitter& emitter)
{
	m_texture.UnLoad(emitter.GetTexPath());
	m_traceDrawList.remove(&emitter);
}

void DsParticleRender::Render() const
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	const DsVec3d camDir =  -m_cam.GetRot().GetAxisZ();

	for (const DsTraceParticleEmitter* pEmitter : m_traceDrawList) {
		const GLuint texId = m_texture.GetTexId(pEmitter->GetTexPath());
		glBindTexture(GL_TEXTURE_2D, texId);
		//�ގ��̐F�͓K��
		const static GLfloat col[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		const static GLfloat spec[] = { 0.0f, 0.0f, 0.0f, 1.0f };	// ���ʔ��ːF
		const static GLfloat ambi[] = { 0.1f, 0.1f, 0.1f, 1.0f };	// ����
		glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambi);
		glMaterialf(GL_FRONT, GL_SHININESS, 30);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, col);
		glColor3fv(col);

		//�p�[�e�B�N�����[�v
		{
			auto func = [this, texId, camDir](const DsSquareParticle& particle) {
				const DsVec2f* pUv = particle.uvPos;
				const DsVec3d* pPos = particle.pos;

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
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);

}