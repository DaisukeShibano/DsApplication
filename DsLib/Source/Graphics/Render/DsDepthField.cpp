#include "DsPch.h"
#include "Graphics/Render/DsDepthField.h"
//他のヘッダ
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
			, m_depTex(0)
			, m_blurTex(0)
			, m_blurFboId(0)
		{
			const int width = static_cast<int>(ren.GetWidth());
			const int height = static_cast<int>(ren.GetHeight());

			{//被写界深度用深度バッファ
				glGenTextures(1, &m_depTex);
				glBindTexture(GL_TEXTURE_2D, m_depTex);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
				glBindTexture(GL_TEXTURE_2D, 0);

				DsGLGenFramebuffers(1, &m_depFboId);
				DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, m_depFboId);
				DsGLFramebufferTexture2D(DS_GL_FRAMEBUFFER, DS_GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depTex, 0);
				DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, 0);
			}

			{//被写界深度用ブラー
				glGenTextures(1, &m_blurTex);
				glBindTexture(GL_TEXTURE_2D, m_blurTex);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				glTexImage2D(GL_TEXTURE_2D, 0, DS_GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, 0);
				glBindTexture(GL_TEXTURE_2D, 0);

				DsGLGenFramebuffers(1, &m_blurFboId);
				DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, m_blurFboId);
				DsGLFramebufferTexture2D(DS_GL_FRAMEBUFFER, DS_GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_blurTex, 0);
				DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, 0);
			}
		}

		virtual ~DepthFieldImp()
		{
			glDeleteTextures(1, &m_depTex);
			DsGLDeleteFramebuffers(1, &m_depFboId);
			glDeleteTextures(1, &m_blurTex);
			DsGLDeleteFramebuffers(1, &m_blurFboId);
		}

		virtual void DepthField() override
		{
			{//被写界深度用depthテクスチャ
				m_shader.EnableShader(SHADER_TYPE::DEPTH_FIELD1);
				m_shader.SetPostEffectParam(0, 1, 2);
				DsGLActiveTexture(DS_GL_TEXTURE2);
				m_postEffectBuffer.BindDepTextureOri();
				
				DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, m_depFboId);
				_Draw();
				DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, 0);
			}

			{//ブラー
				const int width = static_cast<int>(m_render.GetWidth());
				const int height = static_cast<int>(m_render.GetHeight());
				m_shader.EnableShader(SHADER_TYPE::BLUR);
				
				DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, m_blurFboId);
				m_shader.SetBlurParam(DsVec2f(1.0f / (static_cast<float>(width)), 0.0f), 0);
				_Draw();
				m_shader.SetBlurParam(DsVec2f(0.0f, 1.0f / static_cast<float>(height)), 0);
				_Draw();
				DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, 0);
			}

			{//被写界深度合成
				m_shader.EnableShader(SHADER_TYPE::DEPTH_FIELD2);
				m_shader.SetPostEffectParam(0, 1, 2);
				m_shader.DepthFieldParam(3, 4);
				DsGLActiveTexture(DS_GL_TEXTURE0);
				m_postEffectBuffer.BindTexture();
				DsGLActiveTexture(DS_GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, m_depTex);
				DsGLActiveTexture(DS_GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_2D, m_blurTex);

				m_postEffectBuffer.BindFrameBuffer();
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
		GLuint m_depTex;
		GLuint m_depFboId;
		GLuint m_blurTex;
		GLuint m_blurFboId;
	};

}


//static 
DsDepthField* DsDepthField::Create(const DsRender& ren, DsShader& shader, DsPostEffectBuffer& postEffectBuffer)
{
	return new DepthFieldImp(ren, shader, postEffectBuffer);
}



