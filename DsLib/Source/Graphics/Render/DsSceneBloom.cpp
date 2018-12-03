#include "DsPch.h"
#include "Graphics/Render/DsSceneBloom.h"
//他のヘッダ
#include "Graphics/Shader/DsShader.h"
#include "Graphics/GL/DsGLFunc.h"
#include "Graphics/Render/DsRender.h"

#include <gl/GL.h>
#include <gl/GLU.h>

using namespace DsLib;




namespace
{
	class DsSceneBloomImp : public DsSceneBloom
	{
	public:
		DsSceneBloomImp(const DsRender& ren, DsShader& shader)
			: m_render(ren)
			, m_shader(shader)
			, m_fboId(0)
			, m_rboId(0)
		{
			const int width = static_cast<int>(ren.GetWidth());
			const int height = static_cast<int>(ren.GetHeight());

			// FBO作成h
			DsGLGenFramebuffers(1, &m_fboId);
			DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, m_fboId);

			// RBO作成
			DsGLGenRenderbuffers(1, &m_rboId);
			DsGLBindRenderbuffer(DS_GL_RENDERBUFFER, m_rboId);
			DsGLRenderbufferStorage(DS_GL_RENDERBUFFER, GL_RGBA, width, height);
			DsGLBindRenderbuffer(DS_GL_RENDERBUFFER, 0);

			//テクスチャ生成
			glGenTextures(1, &m_texId);
			glBindTexture(GL_TEXTURE_2D, m_texId);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexImage2D(GL_TEXTURE_2D, 0, DS_GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, 0);

			// テクスチャ,RBOと対応付け
			DsGLFramebufferTexture2D(DS_GL_FRAMEBUFFER, DS_GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texId, 0);
			DsGLFramebufferRenderbuffer(DS_GL_FRAMEBUFFER, DS_GL_DEPTH_ATTACHMENT, DS_GL_RENDERBUFFER, m_rboId);

			DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, 0);
			DsGLBindRenderbuffer(DS_GL_RENDERBUFFER, 0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		virtual void Bloom() override
		{

		}

	private:
		const DsRender& m_render;
		DsShader& m_shader;
		GLuint m_fboId;
		GLuint m_rboId;
		GLuint m_texId;
	};

}


//static 
DsSceneBloom* DsSceneBloom::Create(const DsRender& ren, DsShader& shader)
{
	return new DsSceneBloomImp(ren, shader);
}



