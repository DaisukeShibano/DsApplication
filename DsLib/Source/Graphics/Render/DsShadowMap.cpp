#include "DsPch.h"
#include "gl/glew.h"
#include <gl/GL.h>
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
	static const int s_shadowSize = 4096;

	class DsShadowMapImp : public DsShadowMap
	{
	public:
		DsShadowMapImp( const DsRender& ren, DsShader& shader);
		virtual ~DsShadowMapImp();

	public:
		virtual void ActiveShadow() override;
		virtual void DeActiveShadow() override;
		virtual void DrawDepthTex() override;

	private:
		void _SetLightView(const DsLight& light, const DsCamera& cam);

	private:
		const DsRender& m_render;
		DsShader& m_shader;
		GLuint m_iFBODepth;		//!< �������猩���Ƃ��̃f�v�X���i�[����Framebuffer object
		GLuint m_iTexDepth;		//!< m_iFBODepth��attach����e�N�X�`��
		GLsizei m_fDepthSize[2];	//!< �f�v�X���i�[����e�N�X�`���̃T�C�Y
		GLuint m_colorTextureId;
	};

	DsShadowMapImp::DsShadowMapImp( const DsRender& ren, DsShader& shader)
	:m_render(ren)
	,m_shader(shader)
	,m_iFBODepth(0)
	,m_iTexDepth(0)
	,m_colorTextureId(0)
	{
		glewInit();
		if(!glewIsSupported("GL_ARB_depth_texture GL_ARB_shadow ") )
		{
			printf( "ERROR: Support for necessary OpenGL extensions missing.\n" );
			return;
		}

		m_fDepthSize[0] = static_cast<GLsizei>(s_shadowSize);
		m_fDepthSize[1] = static_cast<GLsizei>(s_shadowSize);
	
		// �f�v�X�l�e�N�X�`��
		glActiveTexture(GL_TEXTURE7);
		glGenTextures(1, &m_iTexDepth);
		glBindTexture(GL_TEXTURE_2D, m_iTexDepth);

		// �e�N�X�`���p�����[�^�̐ݒ�
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		GLfloat border_color[4] = {1, 1, 1, 1};
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
	
		// �e�N�X�`���̈�̊m��(GL_DEPTH_COMPONENT��p����)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_fDepthSize[0], m_fDepthSize[1], 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);

		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_fDepthSize[0], m_fDepthSize[1], 0,
		//	GL_RGBA, GL_FLOAT, 0);
		glBindTexture(GL_TEXTURE_2D, 0);




		glGenTextures(1, &m_colorTextureId);
		glBindTexture(GL_TEXTURE_2D, m_colorTextureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F_ARB, m_fDepthSize[0], m_fDepthSize[1], 0, GL_RGB, GL_FLOAT, 0);
		glGenerateMipmapEXT(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);




		// FBO�쐬h
		glGenFramebuffersEXT(1, &m_iFBODepth);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_iFBODepth);

		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	
		// �f�v�X�}�b�v�e�N�X�`����FBO�ɐڑ�
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_iTexDepth, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTextureId, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	//virtual
	DsShadowMapImp::~DsShadowMapImp()
	{
	}

	//virtual
	void DsShadowMapImp::ActiveShadow()
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
		gluPerspective(120, (double)m_fDepthSize[0]/(double)m_fDepthSize[1], cam.GetNear(), cam.GetFar() );
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
		glBindFramebuffer(GL_FRAMEBUFFER, m_iFBODepth);	// FBO�Ƀ����_�����O

		// �J���[�C�f�v�X�o�b�t�@�̃N���A
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearDepth(1.0f);

		// �r���[�|�[�g���V���h�E�}�b�v�̑傫���ɕύX
		glViewport(0, 0, m_fDepthSize[0], m_fDepthSize[1]);
	
		// ���������_�Ƃ��Đݒ�
		//glMatrixMode(GL_PROJECTION);
		//glLoadMatrixf(light_proj);
		//glMatrixMode(GL_MODELVIEW);
		//glLoadMatrixf(light_modelview);//�Ӗ��Ȃ��C������̂ŃR�����g�A�E�g
	
		// �f�v�X�l�ȊO�̐F�̃����_�����O�𖳌��ɂ���
		//glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); //�ڂ����Ŏg��
	
		glPolygonOffset(1.1f, 4.0f);
		glEnable(GL_POLYGON_OFFSET_FILL);

		glEnable(GL_TEXTURE_2D);
	
		glDisable(GL_LIGHTING);
		if(false){
			glDisable(GL_CULL_FACE);
		}
		else{
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
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
		glPushMatrix();
		glActiveTexture(GL_TEXTURE7);
	
		glLoadIdentity();
		glLoadMatrixd(bias);
	
		// �������S���W�ƂȂ�悤�Ƀe�N�X�`���s���ݒ�
		// �e�N�X�`���ϊ��s��Ƀ��f���r���[�C�v���W�F�N�V������ݒ�
		glMultMatrixd(light_proj);
		glMultMatrixd(light_modelview);

		//// ���݂̃��f���r���[�̋t�s��������Ă���
		GLdouble camera_modelview_inv[16];
		DsInverseMatrixDirect16<double>(camera_modelview, camera_modelview_inv);
		glMultMatrixd( camera_modelview_inv );

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// �����ɂ����F�̃����_�����O��L���ɂ���
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); 

		// ���̃r���[�|�[�g�s��ɖ߂�
		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
		// glMatrixMode(GL_TEXTURE)�ŕύX�����}�g���N�X��߂�
		glPopMatrix();

		// �ޔ������Ă��������_�s������ɖ߂�
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		// �f�v�X�e�N�X�`����\��t��
		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, m_iTexDepth);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	//virtual 
	void DsShadowMapImp::DeActiveShadow()
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}


	/*!
	* �f�v�X�}�b�v���e�N�X�`���Ƃ��ĕ\��
	* @param[in] w,h �E�B���h�E�T�C�Y
	*/
	void DsShadowMapImp::DrawDepthTex()
	{
		const float w = m_render.GetWidth();
		const float h = m_render.GetHeight();
		glUseProgram(0);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0, w, 0, h, 0, 1);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glDisable(GL_LIGHTING);
		glColor4f(1, 1, 1, 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_iTexDepth);
		//glBindTexture(GL_TEXTURE_2D, m_colorTextureId);
		glEnable(GL_TEXTURE_2D);
		
		glNormal3d(0, 0, 1);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0); glVertex3f(0.05f*w, 0.05f*h, 0);
		glTexCoord2d(1, 0); glVertex3f(0.05f*w + 100, 0.05f*h, 0);
		glTexCoord2d(1, 1); glVertex3f(0.05f*w + 100, 0.05f*h + 100, 0);
		glTexCoord2d(0, 1); glVertex3f(0.05f*w, 0.05f*h + 100, 0);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}

	void DsShadowMapImp::_SetLightView(const DsLight& light, const DsCamera& cam)
	{
		//���_�ɋ߂��Ȃ�悤�ɕ��s�ړ�������B���z���ȊO���ƌv�Z��ς���K�v����
		const DsVec3d rendPos = cam.GetPos();
		const DsVec3d pos = light.GetPos() + rendPos;
		const DsVec3d lookPos = pos + light.GetDir();
		const DsVec3d up = DsVec3d::Up();
		gluLookAt(pos.x, pos.y, pos.z,
			lookPos.x, lookPos.y, lookPos.z,
			up.x, up.y, up.z);

		//�e�̃����_�����O�͈͂Ǝ��_�̃����_�����O�͈͂Əd�Ȃ�Ȃ����Ƃ�����̂ŁA�d�Ȃ�悤�v�Z����K�v������
	}
}

//static
DsShadowMap& DsShadowMap::Create( const DsRender& ren, DsShader& shader )
{
	DsShadowMap* ret = new DsShadowMapImp( ren, shader);
	if( NULL == ret )
	{
		DsError::Panic("");
	}
	return *ret;
}