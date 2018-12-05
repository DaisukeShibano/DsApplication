#include "DsPch.h"
#ifndef _DS_GL_FUNC_
#include "Graphics/GL/DsGLFunc.h"
#endif
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Graphics/Render/DsShadowMap.h"
#include "Graphics/Render/DsRender.h"
#include "Graphics/Shader/DsShader.h"
#include "Graphics/Camera/DsCamera.h"
#include "Graphics/Light/DsLight.h"
#include "Graphics/Light/DsLightMan.h"
#include "Math/DsInverseMatrix.h"



using namespace DsLib;

namespace
{
	static const int s_shadowSize = 1024;
	static const double BLUR_COEF = 1.0;

	class DsShadowMapImp : public DsShadowMap
	{
	public:
		DsShadowMapImp( const DsRender& ren, DsShader& shader);
		virtual ~DsShadowMapImp();

	public:
		virtual void DrawShadow() override;
		virtual void DrawDepthTex() override;

	private:
		void _SetLightView(const DsLight& light, const DsCamera& cam);
		void _Blur();

	private:
		const DsRender& m_render;
		DsShader& m_shader;
		GLuint m_iFBODepth;		//!< �������猩���Ƃ��̃f�v�X���i�[����Framebuffer object
		GLuint m_iTexDepth;		//!< m_iFBODepth��attach����e�N�X�`��
		GLsizei m_fDepthSize[2];	//!< �f�v�X���i�[����e�N�X�`���̃T�C�Y
		GLuint m_colorTextureId;
		GLuint m_depthrRboId;
		GLuint m_blurFboId;
		GLuint m_blurFboIdColorTextureId;
	};

	DsShadowMapImp::DsShadowMapImp( const DsRender& ren, DsShader& shader)
	:m_render(ren)
	,m_shader(shader)
	,m_iFBODepth(0)
	,m_iTexDepth(0)
	,m_colorTextureId(0)
	,m_blurFboId(0)
	,m_blurFboIdColorTextureId(0)
	{
		m_fDepthSize[0] = static_cast<GLsizei>(s_shadowSize);
		m_fDepthSize[1] = static_cast<GLsizei>(s_shadowSize);

		// �f�v�X�l�e�N�X�`��
		glGenTextures(1, &m_iTexDepth);
		glBindTexture(GL_TEXTURE_2D, m_iTexDepth);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, DS_GL_CLAMP_TO_BORDER);//�V���h�E�e�N�X�`���͈̔͂𒴂����ꍇ�A�����ł����Ăق����̂ŁA�{�[�_�[�J���[�ɂȂ�悤��
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, DS_GL_CLAMP_TO_BORDER);
		GLfloat border_color[4] = {1, 1, 1, 1};
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_fDepthSize[0], m_fDepthSize[1], 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
		glBindTexture(GL_TEXTURE_2D, 0);


		//�f�v�X�ȗp�r�Ȃ񂾂��Ǖ��������g���̂ŃJ���[
		glGenTextures(1, &m_colorTextureId);
		glBindTexture(GL_TEXTURE_2D, m_colorTextureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, DS_GL_CLAMP_TO_BORDER);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, DS_GL_CLAMP_TO_BORDER);
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
		glTexImage2D(GL_TEXTURE_2D, 0, DS_GL_RGB32F, m_fDepthSize[0], m_fDepthSize[1], 0, GL_RGB, GL_FLOAT, 0);
		DsGLGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);


		// FBO�쐬h
		DsGLGenFramebuffers(1, &m_iFBODepth);
		DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, m_iFBODepth);
		//glDrawBuffer(GL_NONE);
		//glReadBuffer(GL_NONE);
		// �f�v�X�}�b�v�e�N�X�`����FBO�ɐڑ�
		DsGLFramebufferTexture2D(DS_GL_FRAMEBUFFER, DS_GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_iTexDepth, 0);
		DsGLFramebufferTexture2D(DS_GL_FRAMEBUFFER, DS_GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTextureId, 0);
		GLenum FBOstatus = DsGLCheckFramebufferStatus(DS_GL_FRAMEBUFFER);
		if (FBOstatus != DS_GL_FRAMEBUFFER_COMPLETE) {
			printf("DS_GL_FRAMEBUFFER_COMPLETE failed for shadowmap FBO, CANNOT use FBO\n");
		}
		DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, 0);


		// �u���[�p
		DsGLGenFramebuffers(1, &m_blurFboId);
		DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, m_blurFboId);
		glGenTextures(1, &m_blurFboIdColorTextureId);
		glBindTexture(GL_TEXTURE_2D, m_blurFboIdColorTextureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexImage2D(GL_TEXTURE_2D, 0, DS_GL_RGB32F, static_cast<GLsizei>(m_fDepthSize[0]*BLUR_COEF), static_cast<GLsizei>(m_fDepthSize[1]*BLUR_COEF), 0, GL_RGB, GL_FLOAT, 0);

		DsGLFramebufferTexture2D(DS_GL_FRAMEBUFFER, DS_GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_blurFboIdColorTextureId, 0);
		FBOstatus = DsGLCheckFramebufferStatus(DS_GL_FRAMEBUFFER);
		if (FBOstatus != DS_GL_FRAMEBUFFER_COMPLETE) {
			printf("DS_GL_FRAMEBUFFER_COMPLETE failed for blur FBO, CANNOT use FBO\n");
		}
		glBindTexture(GL_TEXTURE_2D, 0);
		DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, 0);
	}

	//virtual
	DsShadowMapImp::~DsShadowMapImp()
	{
	}

	//virtual
	void DsShadowMapImp::DrawShadow()
	{
		double light_proj[16], camera_proj[16];
		double light_modelview[16], camera_modelview[16];
		const DsLight& light = m_render.RefLight();
		const DsCamera& cam = m_render.RefCam();

		//
		// ���݂̎��_�s��C�����s����擾 or �쐬
		//
		glMatrixMode(GL_PROJECTION);

		// ���_�v���W�F�N�V�����s��̎擾
		glGetDoublev(GL_PROJECTION_MATRIX, camera_proj);
		glPushMatrix();	// ���̃v���W�F�N�V�����s���ޔ������Ă���

		// �����v���W�F�N�V�����s��̐����Ǝ擾
		glLoadIdentity();
		gluPerspective(50, (double)m_fDepthSize[0]/(double)m_fDepthSize[1], cam.GetNear(), cam.GetFar() );
		//glOrtho�ɓ����left�Ƃ�bottom�͉e������͈͂̃T�C�Y�ɂȂ�B���W�̓��f���r���[���W�B�V���h�E�}�b�v�̃T�C�Y�ł͂Ȃ��̂Œ���
		//glOrtho(-2, 2, -2, 2, 1, 1000);
		glGetDoublev(GL_PROJECTION_MATRIX, light_proj);
		
		glMatrixMode(GL_MODELVIEW);

		// ���_���f���r���[�s��̎擾
		glGetDoublev(GL_MODELVIEW_MATRIX, camera_modelview);
		glPushMatrix();	// ���̃��f���r���[�s���ޔ������Ă���

		// �������f���r���[�s��̐����Ǝ擾
		glLoadIdentity();
		_SetLightView(light, cam);
		
		glGetDoublev(GL_MODELVIEW_MATRIX, light_modelview);

		// ���̃r���[�|�[�g������Ŗ߂����߂Ɋm��
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);


		// �������烌���_�����O���ăV���h�E�}�b�v�𐶐�
		DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, m_iFBODepth);	// FBO�Ƀ����_�����O

		// �J���[�C�f�v�X�o�b�t�@�̃N���A
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearDepth(1.0f);

		// �r���[�|�[�g���V���h�E�}�b�v�̑傫���ɕύX
		glViewport(0, 0, m_fDepthSize[0], m_fDepthSize[1]);
	
		// ���������_�Ƃ��Đݒ�
		//glMatrixMode(GL_PROJECTION);
		//glLoadMatrixd(light_proj);
		//glMatrixMode(GL_MODELVIEW);
		//glLoadMatrixd(light_modelview);//�������ɂȂ��ĂĈӖ��Ȃ��C������̂ŃR�����g�A�E�g
	
		// �f�v�X�l�ȊO�̐F�̃����_�����O�𖳌��ɂ���
		//glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);//rg���ڂ����̌v�Z�Ŏg��
		
		glPolygonOffset(1.1f, 4.0f);
		glEnable(GL_POLYGON_OFFSET_FILL);

		glEnable(GL_TEXTURE_2D);
	
		glDisable(GL_LIGHTING);
		if(false){
			//����
			glDisable(GL_CULL_FACE);
		}
		else{
			//����
			//glEnable(GL_CULL_FACE);
			//glCullFace(GL_FRONT);
		}
		
		//�����ŉe�p�I�u�W�F�g�`��
		m_shader.EnableShader(SHADER_TYPE::SHADOW_MAP);
		m_render.RendPolygon();
		m_shader.DisableShader();

		glDisable(GL_POLYGON_OFFSET_FILL);
	
		const double bias[16] = {	0.5, 0.0, 0.0, 0.0, 
									0.0, 0.5, 0.0, 0.0,
									0.0, 0.0, 0.5, 0.0,
									0.5, 0.5, 0.5, 1.0 };
	

	
		// �e�N�X�`�����[�h�Ɉڍs
		glMatrixMode(GL_TEXTURE);
		DsGLActiveTexture(DS_GL_TEXTURE7);
		
		glLoadIdentity();
		glLoadMatrixd(bias);

		// �������S���W�ƂȂ�悤�Ƀe�N�X�`���s���ݒ�
		// �e�N�X�`���ϊ��s��Ƀ��f���r���[�C�v���W�F�N�V������ݒ�
		glMultMatrixd(light_proj);
		glMultMatrixd(light_modelview);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		_Blur();
		DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, 0);

		// �����ɂ����F�̃����_�����O��L���ɂ���
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); 

		// ���̃r���[�|�[�g�s��ɖ߂�
		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
		// glMatrixMode(GL_TEXTURE)�ŕύX�����}�g���N�X��߂�
		//glPopMatrix();

		// �ޔ������Ă��������_�s������ɖ߂�
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		// �f�v�X�e�N�X�`����\��t��
		glEnable(GL_TEXTURE_2D);
		DsGLActiveTexture(DS_GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, m_iTexDepth);
		DsGLActiveTexture(DS_GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, m_colorTextureId);
		DsGLGenerateMipmap(GL_TEXTURE_2D);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		DsGLActiveTexture(DS_GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glClearColor(0.7f, 0.8f, 1.0f, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	/*!
	* �f�v�X�}�b�v���e�N�X�`���Ƃ��ĕ\��
	* @param[in] w,h �E�B���h�E�T�C�Y
	*/
	void DsShadowMapImp::DrawDepthTex()
	{
		const double w = m_render.GetWidth();
		const double h = m_render.GetHeight();
		m_shader.DisableShader();
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0, w, 0, h, 0, 1);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glDisable(GL_LIGHTING);
		glColor4f(1, 1, 1, 1);
		DsGLActiveTexture(DS_GL_TEXTURE0);
		glEnable(GL_TEXTURE_2D);
		//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		//glBindTexture(GL_TEXTURE_2D, m_iTexDepth);
		glBindTexture(GL_TEXTURE_2D, m_colorTextureId);
		//glBindTexture(GL_TEXTURE_2D, m_blurFboIdColorTextureId);
		glNormal3d(0, 0, 1);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0); glVertex3d(0.05*w, 0.05*h, 0);
		glTexCoord2d(1, 0); glVertex3d(0.05*w + 100, 0.05*h, 0);
		glTexCoord2d(1, 1); glVertex3d(0.05*w + 100, 0.05*h + 100, 0);
		glTexCoord2d(0, 1); glVertex3d(0.05*w, 0.05*h + 100, 0);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		m_shader.DisableShader();
	}

	void DsShadowMapImp::_Blur()
	{
		m_shader.EnableShader(SHADER_TYPE::BLUR);

		DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, m_blurFboId);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glClearDepth(1.0f);
		//m_blurFboId�։������ɂڂ����Bm_blurFboId��m_blurFboIdColorTextureId�ƕR�Â��Ă���̂ł����Ɍ��ʂ��i�[�����B�����ڂ������߂̈ꎞ�e�N�X�`��
		glViewport(0, 0, static_cast<GLsizei>(m_fDepthSize[0] * BLUR_COEF), static_cast<GLsizei>(m_fDepthSize[1] * BLUR_COEF));
		
		m_shader.SetBlurParam(DsVec2f( 1.0f / (static_cast<float>(m_fDepthSize[0])*static_cast<float>( BLUR_COEF) ), 0.0f),		// Bluring horinzontaly
									0);
		DsGLActiveTexture(DS_GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_colorTextureId);//�V���h�E�}�b�v(6��7������6�̕�)���e�N�X�`���Ƃ��ăo�C���h

		//Preparing to draw quad
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-m_fDepthSize[0] / 2, m_fDepthSize[0] / 2, -m_fDepthSize[1] / 2, m_fDepthSize[1] / 2, 1, 20);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Drawing quad 
		glTranslated(0, 0, -5);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0); glVertex3f(static_cast<float>(-m_fDepthSize[0] / 2), static_cast<float>(-m_fDepthSize[1] / 2), 0);
		glTexCoord2d(1, 0); glVertex3f(static_cast<float>(m_fDepthSize[0] / 2), static_cast<float>(-m_fDepthSize[1] / 2), 0);
		glTexCoord2d(1, 1); glVertex3f(static_cast<float>(m_fDepthSize[0] / 2), static_cast<float>(m_fDepthSize[1] / 2), 0);
		glTexCoord2d(0, 1); glVertex3f(static_cast<float>(-m_fDepthSize[0] / 2), static_cast<float>(m_fDepthSize[1] / 2), 0);
		glEnd();
		//DsGLGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Bluring vertically
		DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, m_iFBODepth);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glClearDepth(1.0f);
		//m_blurFboIdColorTextureId �ŉ����ɂڂ�����Ă�̂ŁA�c�����ɂڂ��������̂�m_iFBODepth�ŕ`��Bm_iFBODepth��m_colorTextureId�ɕR�Â��Ă���̂ōŏI�I�ȃV���h�E�}�b�v������
		glViewport(0, 0, m_fDepthSize[0], m_fDepthSize[1]);
		m_shader.SetBlurParam( DsVec2f(0.0f, 1.0f / static_cast<float>(m_fDepthSize[1])),
									0);
		glBindTexture(GL_TEXTURE_2D, m_blurFboIdColorTextureId);//m_blurFboIdColorTextureId

		glBegin(GL_QUADS);
		glTexCoord2d(0, 0); glVertex3f(static_cast<float>(-m_fDepthSize[0] / 2), static_cast<float>(-m_fDepthSize[1] / 2), 0);
		glTexCoord2d(1, 0); glVertex3f(static_cast<float>(m_fDepthSize[0] / 2), static_cast<float>(-m_fDepthSize[1] / 2), 0);
		glTexCoord2d(1, 1); glVertex3f(static_cast<float>(m_fDepthSize[0] / 2), static_cast<float>(m_fDepthSize[1] / 2), 0);
		glTexCoord2d(0, 1); glVertex3f(static_cast<float>(-m_fDepthSize[0] / 2), static_cast<float>(m_fDepthSize[1] / 2), 0);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);
		DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, 0);

		m_shader.DisableShader();
	}

	void DsShadowMapImp::_SetLightView(const DsLight& light, const DsCamera& cam)
	{
		//���_�ɋ߂��Ȃ�悤�ɕ��s�ړ�������B
		const DsVec3d rendPos = cam.GetPos();
		const DsVec3d pos = light.GetPos() + rendPos;
		const DsVec3d lookPos = pos + light.GetDir();
		const DsVec3d up = DsVec3d::Up();
		gluLookAt(pos.x, pos.y, pos.z,
			lookPos.x, lookPos.y, lookPos.z,
			up.x, up.y, up.z);
	}
}

//static
DsShadowMap& DsShadowMap::Create( const DsRender& ren, DsShader& shader )
{
	DsShadowMap* ret = new DsShadowMapImp( ren, shader);
	DS_ASSERT(ret, "�������m�ێ��s");
	return *ret;
}