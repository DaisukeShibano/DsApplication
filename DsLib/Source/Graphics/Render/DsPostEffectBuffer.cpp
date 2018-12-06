#include "DsPch.h"
#include "Graphics/Render/DsPostEffectBuffer.h"
//他のヘッダ
#include "Graphics/Shader/DsShader.h"
#include "Graphics/GL/DsGLFunc.h"
#include "Graphics/Render/DsRender.h"

#include <gl/GL.h>
#include <gl/GLU.h>


using namespace DsLib;


namespace
{
	class DsPostEffectBufferImp : public DsPostEffectBuffer
	{
	public:
		DsPostEffectBufferImp(const DsRender& ren, DsShader& shader)
			: m_render(ren)
			, m_shader(shader)
			, m_fboId(0)
			, m_texId(0)
			, m_texOriId(0)
			, m_fboOriId(0)

		{
			const int width = static_cast<int>(ren.GetWidth());
			const int height = static_cast<int>(ren.GetHeight());
			glGenTextures(1, &m_texId);
			glBindTexture(GL_TEXTURE_2D, m_texId);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexImage2D(GL_TEXTURE_2D, 0, DS_GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, 0);
			glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, width, height, 0);
			glBindTexture(GL_TEXTURE_2D, 0);
			
			DsGLGenFramebuffers(1, &m_fboId);
			DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, m_fboId);
			DsGLFramebufferTexture2D(DS_GL_FRAMEBUFFER, DS_GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texId, 0);
			DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, 0);

			glGenTextures(1, &m_texOriId);
			glBindTexture(GL_TEXTURE_2D, m_texOriId);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexImage2D(GL_TEXTURE_2D, 0, DS_GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, 0);
			glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, width, height, 0);
			glBindTexture(GL_TEXTURE_2D, 0);

			//DsGLGenFramebuffers(1, &m_fboOriId);
			//DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, m_fboOriId);
			//DsGLFramebufferTexture2D(DS_GL_FRAMEBUFFER, DS_GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texOriId, 0);
			//DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, 0);
		}

		~DsPostEffectBufferImp()
		{
			glDeleteTextures(1, &m_texId);
			DsGLDeleteFramebuffers(1, &m_fboId);
			glDeleteTextures(1, &m_texOriId);
		}

		virtual void CopyFrameBuffer() override
		{
			const int width = static_cast<int>(m_render.GetWidth());
			const int height = static_cast<int>(m_render.GetHeight());
			DsGLActiveTexture(DS_GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_texId);
			glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, width, height, 0);
			glBindTexture(GL_TEXTURE_2D, 0);

			DsGLActiveTexture(DS_GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_texOriId);
			glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, width, height, 0);
			glBindTexture(GL_TEXTURE_2D, 0);

			DsGLActiveTexture(DS_GL_TEXTURE0);
		}

		virtual void RenderFrame() override
		{
			const double w = m_render.GetWidth();
			const double h = m_render.GetHeight();

			m_shader.DisableShader();//テクスチャ１枚描画だけなので特別なシェーダーはいらない

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
			glBindTexture(GL_TEXTURE_2D, m_texId);
			glNormal3d(0, 0, 1);
			glBegin(GL_QUADS);
			glTexCoord2d(0, 0); glVertex3d(0, 0, 0);
			glTexCoord2d(1, 0); glVertex3d(w, 0, 0);
			glTexCoord2d(1, 1); glVertex3d(w, h, 0);
			glTexCoord2d(0, 1); glVertex3d(0, h, 0);
			glEnd();
			glBindTexture(GL_TEXTURE_2D, 0);
			glDisable(GL_TEXTURE_2D);
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
		}


		virtual void BindTexture() override
		{
			glBindTexture(GL_TEXTURE_2D, m_texId);
		}

		virtual void UnbindTexture() override
		{
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		virtual void BindFrameBuffer() override
		{
			DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, m_fboId);
		}

		virtual void UnbindFrameBuffer() override
		{
			DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, 0);
		}

		virtual void BindTextureOri() override
		{
			glBindTexture(GL_TEXTURE_2D, m_texOriId);
		}

		virtual void BindFrameBufferOri() override
		{
			//DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, m_fboOriId);
		}

		virtual void UnbindFrameBufferOri() override
		{
			//DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, 0);
		}


	public:
		virtual void DbgDraw() override
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
			glBindTexture(GL_TEXTURE_2D, m_texId);
			glNormal3d(0, 0, 1);
			glBegin(GL_QUADS);
			glTexCoord2d(0, 0); glVertex3d(0.05*w, 0.05*h, 0);
			glTexCoord2d(1, 0); glVertex3d(0.05*w + 200, 0.05*h, 0);
			glTexCoord2d(1, 1); glVertex3d(0.05*w + 200, 0.05*h + 200, 0);
			glTexCoord2d(0, 1); glVertex3d(0.05*w, 0.05*h + 200, 0);
			glEnd();
			glBindTexture(GL_TEXTURE_2D, 0);
			glDisable(GL_TEXTURE_2D);
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			
			m_shader.DisableShader();
		}

	private:
		const DsRender& m_render;
		DsShader& m_shader;
		GLuint m_fboId;
		GLuint m_texId;
		GLuint m_texOriId;
		GLuint m_fboOriId;
	};
}

//static
DsPostEffectBuffer* DsPostEffectBuffer::Create(const DsRender& ren, DsShader& shader)
{
	return new DsPostEffectBufferImp(ren, shader);
}