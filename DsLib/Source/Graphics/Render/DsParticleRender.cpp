#include "DsPch.h"

#include <gl/GL.h>
#include "Graphics/GL/DsGLFunc.h"
#include <gl/GLU.h>

#ifndef _DS_PARTICLE_RENDER_
#include "Graphics/Render/DsParticleRender.h"
#endif
//他のヘッダ
#include "Graphics/Effect/Particle/DsParticleEmitter.h"
#ifndef _DS_CAMERA_H_
#include "Graphics/Camera/DsCamera.h"
#endif
#include "Graphics/Shader/DsShader.h"

using namespace DsLib;


DsParticleRender::DsParticleRender(const DsCamera& cam)
	: m_emitterList()
	, m_texture()
	, m_cam(cam)
	, m_pShader(NULL)
{
}

DsParticleRender::~DsParticleRender()
{
}

void DsParticleRender::Initialize(DsShader& shader)
{
	m_pShader = &shader;
}

void DsParticleRender::Register(const DsParticleEmitter& emitter)
{
	m_emitterList.push_back(&emitter);
	m_texture.Load(emitter.GetAlbedoTexPath());
	if (emitter.IsUseNormalMap()) {
		m_texture.Load(emitter.GetNormalTexPath());
	}
}

void DsParticleRender::UnRegister(const DsParticleEmitter& emitter)
{
	m_texture.UnLoad(emitter.GetAlbedoTexPath());
	if (emitter.IsUseNormalMap()) {
		m_texture.UnLoad(emitter.GetNormalTexPath());
	}
	m_emitterList.remove(&emitter);
}

void DsParticleRender::Render() const
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);

	{//モデルプロジェクション行列セット
		const DsMat44f modelMat = DsMat44f::Identity();
		DsMat44f projMat = DsMat44f::Identity();
		glGetFloatv(GL_PROJECTION_MATRIX, projMat.mat);

		DsMat44f modelProjMat = projMat;

		DsMat44f modelProjInv = DsMat44f::Identity();
		DsInverseMatrix<4, float>(modelProjMat.mat, modelProjInv.mat);

		m_pShader->SetModelTransform(modelMat.mat);
		m_pShader->SetModelViewProjectionTransform(modelProjMat.mat);
		m_pShader->SetModelViewProjectionInverseTransform(modelProjInv.mat);
	}

	const DsVec3d camDir = -m_cam.GetRot().GetAxisZ();

	for (const DsParticleEmitter* pEmitter : m_emitterList) {
		DsGLActiveTexture(DS_GL_TEXTURE0);
		const GLuint albedoTexId = m_texture.GetTexId(pEmitter->GetAlbedoTexPath());
		glBindTexture(GL_TEXTURE_2D, albedoTexId);

		//ノーマルマップのある材質だけ有効
		bool isUseNormal = false;
		if (pEmitter->IsUseNormalMap()) {
			const GLuint normalTexId = m_texture.GetTexId(pEmitter->GetNormalTexPath());
			if (m_texture.IsValidId(normalTexId)) {
				DsGLActiveTexture(DS_GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, normalTexId);
				isUseNormal = true;
			}
		}
		m_pShader->SetUseNormalMap(isUseNormal);

		bool isUseSpecular = false;
		if (pEmitter->IsUseSpecularMap()) {
			const GLuint specularTexId = m_texture.GetTexId(pEmitter->GetSpecularTexPath());
			if (m_texture.IsValidId(specularTexId)) {
				DsGLActiveTexture(DS_GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, specularTexId);
				isUseSpecular = true;
			}
		}
		m_pShader->SetUseSpecularMap(isUseSpecular);

		//戻す
		DsGLActiveTexture(DS_GL_TEXTURE0);

		//材質の色は適当
		const GLfloat diff[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		const GLfloat spec[] = { 1.0f, 1.0f, 1.0f, 1.0f };	// 鏡面反射色
		const GLfloat ambi[] = { 0.0f, 0.0f, 0.0f, 1.0f };	// 環境光
		const GLfloat emis[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambi);
		glMaterialf(GL_FRONT, GL_SHININESS, 30);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diff);
		glMaterialfv(GL_FRONT, GL_EMISSION, emis);

		//パーティクルループ
		{
			auto func = [this, camDir, pEmitter](const DsSquareParticle& particle) {
				const DsVec2f* pUv = particle.uvPos;
				const DsVec3d* pPos = particle.pos;
				const double alpha = pEmitter->GetAlpha(particle.lifeTime);
				const GLfloat col[] = { 1.0f, 1.0f, 1.0f, static_cast<float>(alpha) };
				
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

	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);

}