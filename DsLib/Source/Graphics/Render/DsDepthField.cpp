#include "DsPch.h"
#include "Graphics/Render/DsDepthField.h"
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
	class DepthFieldImp : public DsDepthField
	{
	public:
		DepthFieldImp(const DsRender& ren, DsShader& shader, DsPostEffectBuffer& postEffectBuffer)
			: m_render(ren)
			, m_shader(shader)
			, m_postEffectBuffer(postEffectBuffer)
		{
		}

		virtual ~DepthFieldImp()
		{
		}

		virtual void DepthField() override
		{
			{//��ʊE�[�x�pdepth�e�N�X�`��
				m_shader.EnableShader(SHADER_TYPE::DEPTH_FIELD1);
				m_shader.SetPostEffectParam(0, 1, 2);
				DsGLActiveTexture(DS_GL_TEXTURE2);
				m_postEffectBuffer.BindDepTextureOri();
				
				m_postEffectBuffer.BindFrameBuffer();//�����ɐ[�x����������
				_Draw();
				m_postEffectBuffer.UnbindFrameBuffer();
			}

			{//�u���[
				const int width = static_cast<int>(m_render.GetWidth());
				const int height = static_cast<int>(m_render.GetHeight());

				const DsShader::BlurParam blurParam = DsShader::GetBlurParam(29, 100.0f);

				m_postEffectBuffer.BindTmpFrameBuffer2();//�������֊i�[
				DsGLActiveTexture(DS_GL_TEXTURE0);
				m_postEffectBuffer.BindCurrentResultTexture();//���摜���Q��
				m_shader.EnableShader(SHADER_TYPE::BLUR_HORIZON);
				m_shader.SetBlurParam(1.0f / (static_cast<float>(width)), 0, blurParam);
				_Draw();

				m_postEffectBuffer.BindTmpFrameBuffer3();//���ʂ��i�[
				DsGLActiveTexture(DS_GL_TEXTURE0);
				m_postEffectBuffer.BindTmpColorTexture2();//���������Q��
				m_shader.EnableShader(SHADER_TYPE::BLUR_VERTICAL);
				m_shader.SetBlurParam(1.0f / static_cast<float>(height), 0, blurParam);
				_Draw();

				DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, 0);
				m_postEffectBuffer.UnbindFrameBuffer();
			}

			{//��ʊE�[�x����
				m_shader.EnableShader(SHADER_TYPE::DEPTH_FIELD2);
				m_shader.SetPostEffectParam(0, 1, 2);
				m_shader.DepthFieldParam(3, 4);
				DsGLActiveTexture(DS_GL_TEXTURE0);
				m_postEffectBuffer.BindCurrentResultTexture();//���H�O
				DsGLActiveTexture(DS_GL_TEXTURE3);
				m_postEffectBuffer.BindTexture();//�[�x
				DsGLActiveTexture(DS_GL_TEXTURE4);
				m_postEffectBuffer.BindTmpColorTexture3();//�u���[

				m_postEffectBuffer.BindTmpFrameBuffer2();
				_Draw();
				m_postEffectBuffer.UnbindFrameBuffer();

				m_postEffectBuffer.SetCurrentResult(2);//�Q�Ԏg���Ă��Ȃ��̂ŏo�͐��
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
DsDepthField* DsDepthField::Create(const DsRender& ren, DsShader& shader, DsPostEffectBuffer& postEffectBuffer)
{
	return new DepthFieldImp(ren, shader, postEffectBuffer);
}



