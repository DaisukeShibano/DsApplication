#include "DsPch.h"
#include "Graphics/Render/DsPostEffectBuffer.h"
//‘¼‚Ìƒwƒbƒ_
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
			, m_texId(0)
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
		}

		~DsPostEffectBufferImp()
		{
			glDeleteTextures(1, &m_texId);
		}

		virtual void CopyFrameBuffer() override
		{
			//DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, 0);
			const int width = static_cast<int>(m_render.GetWidth());
			const int height = static_cast<int>(m_render.GetHeight());
			glBindTexture(GL_TEXTURE_2D, m_texId);
			glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, width, height, 0);
		}

		virtual void BindTexture() override
		{
			glBindTexture(GL_TEXTURE_2D, m_texId);
		}

		virtual void UnbindTexture() override
		{
			glBindTexture(GL_TEXTURE_2D, 0);
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

	private:
		const DsRender& m_render;
		DsShader& m_shader;
		GLuint m_fboId;
		GLuint m_texId;
		GLuint m_dboId;
	};
}

//static
DsPostEffectBuffer* DsPostEffectBuffer::Create(const DsRender& ren, DsShader& shader)
{
	return new DsPostEffectBufferImp(ren, shader);
}