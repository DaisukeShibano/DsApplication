#include "DsPch.h"
#include "Graphics/Render/DsSceneBloom.h"
//���̃w�b�_
#include "Graphics/Shader/DsShader.h"
#include "Graphics/GL/DsGLFunc.h"
#include "Graphics/Render/DsRender.h"
#include "Graphics/Render/DsPostEffectBuffer.h"

#include <gl/GL.h>
#include <gl/GLU.h>

using namespace DsLib;




namespace
{
	class DsSceneBloomImp : public DsSceneBloom
	{
	public:
		DsSceneBloomImp(const DsRender& ren, DsShader& shader, DsPostEffectBuffer& postEffectBuffer)
			: m_render(ren)
			, m_shader(shader)
			, m_postEffectBuffer(postEffectBuffer)
		{
			
		}

		virtual ~DsSceneBloomImp()
		{
		}

		virtual void Bloom() override
		{
			{//���P�x���o
				m_shader.EnableShader(SHADER_TYPE::BLOOM1);
				m_shader.SetPostEffectParam(0, 1, 2);
				DsGLActiveTexture(DS_GL_TEXTURE0);
				m_postEffectBuffer.BindTextureOri();//��ԍŏ��̃I���W�i���J���[�o�b�t�@
				m_postEffectBuffer.BindFrameBuffer();
				_Draw();
				m_postEffectBuffer.UnbindFrameBuffer();
			}
			{//�u���[
				const int width = static_cast<int>(m_render.GetWidth());
				const int height = static_cast<int>(m_render.GetHeight());
				
				m_shader.EnableShader(SHADER_TYPE::BLUR_HORIZON);
				const DsShader::BlurParam blurParam = DsShader::GetBlurParam(19);
				m_shader.SetBlurParam(1.0f / (static_cast<float>(width)), 0, blurParam);
				DsGLActiveTexture(DS_GL_TEXTURE0);
				m_postEffectBuffer.BindTexture();//0�ԖڂɋP�x���o�����e�N�X�`�����o�C���h
				m_postEffectBuffer.BindTmpFrameBuffer1();//���ʂ��������o�b�t�@��
				_Draw();
				m_postEffectBuffer.UnbindFrameBuffer();

				m_shader.EnableShader(SHADER_TYPE::BLUR_VERTICAL);
				m_shader.SetBlurParam(1.0f / static_cast<float>(height), 0, blurParam);
				DsGLActiveTexture(DS_GL_TEXTURE0);
				m_postEffectBuffer.BindTmpColorTexture1();//0�Ԗڂɉ��������H�ς݂��o�C���h
				m_postEffectBuffer.BindTmpFrameBuffer2();//���ʂ��c�����o�b�t�@��
				_Draw();
				m_postEffectBuffer.UnbindFrameBuffer();
			}
			{//����
				m_shader.EnableShader(SHADER_TYPE::BLOOM2);
				m_shader.SetPostEffectParam(0, 1, 2);
				DsGLActiveTexture(DS_GL_TEXTURE0);
				m_postEffectBuffer.BindTmpColorTexture2();//0�Ԗڂɏc���������H�ς݂��o�C���h
				DsGLActiveTexture(DS_GL_TEXTURE1);
				m_postEffectBuffer.BindTextureOri();//1�Ԃ̃e�X�N�`���ɉ��H�O���o�C���h
				//2�͎g��Ȃ�

				m_postEffectBuffer.BindTmpFrameBuffer1();//���̃|�X�g�G�t�F�N�g�֓n��
				_Draw();
				m_postEffectBuffer.UnbindFrameBuffer();
			}

			m_postEffectBuffer.UnbindTexture();
			DsGLActiveTexture(DS_GL_TEXTURE0);
			m_shader.DisableShader();
		}

	private:
		void _Draw()
		{
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			glOrtho(0, 1, 0, 1, 0, 1);
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();

			glBegin(GL_QUADS);
			glTexCoord2d(0, 0); glVertex3d(0, 0, 0);
			glTexCoord2d(1, 0); glVertex3d(1, 0, 0);
			glTexCoord2d(1, 1); glVertex3d(1, 1, 0);
			glTexCoord2d(0, 1); glVertex3d(0, 1, 0);
			glEnd();

			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
		}

	public:
		virtual void DbgDraw() override
		{

		}

	private:
		const DsRender& m_render;
		DsShader& m_shader;
		DsPostEffectBuffer& m_postEffectBuffer;
	};

}


//static 
DsSceneBloom* DsSceneBloom::Create(const DsRender& ren, DsShader& shader, DsPostEffectBuffer& postEffectBuffer)
{
	return new DsSceneBloomImp(ren, shader, postEffectBuffer);
}



