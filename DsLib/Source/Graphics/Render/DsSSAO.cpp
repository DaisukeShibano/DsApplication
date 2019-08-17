#include "DsPch.h"
#include "Graphics/Render/DsSSAO.h"
//‘¼‚Ìƒwƒbƒ_
#include "Graphics/Shader/DsShader.h"
#include "Graphics/GL/DsGLFunc.h"
#include "Graphics/Render/DsRender.h"
#include "Graphics/Render/DsPostEffectBuffer.h"

#include <gl/GL.h>
#include <gl/GLU.h>

using namespace DsLib;




namespace
{
	class DsSSAOImp : public DsSSAO
	{
	public:
		DsSSAOImp(const DsRender& ren, DsShader& shader, DsPostEffectBuffer& postEffectBuffer)
			: m_render(ren)
			, m_shader(shader)
			, m_postEffectBuffer(postEffectBuffer)
		{
		}

		virtual void SSAO() override
		{
			m_shader.EnableShader(SHADER_TYPE::SSAO);
			m_shader.SetPostEffectParam(0, 1, 2);
			DsGLActiveTexture(DS_GL_TEXTURE0);
			m_postEffectBuffer.BindTextureOri();
			DsGLActiveTexture(DS_GL_TEXTURE2);
			m_postEffectBuffer.BindDepTextureOri();
			m_postEffectBuffer.BindFrameBuffer();
			_Draw();
			m_postEffectBuffer.UnbindFrameBuffer();
			m_postEffectBuffer.UnbindTexture();
			DsGLActiveTexture(DS_GL_TEXTURE0);
			m_shader.DisableShader();

			m_postEffectBuffer.SetCurrentResult(0);//0”Ô–Ú‚ðŒ‹‰Ê‚É
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
DsSSAO* DsSSAO::Create(const DsRender& ren, DsShader& shader, DsPostEffectBuffer& postEffectBuffer)
{
	return new DsSSAOImp(ren, shader, postEffectBuffer);
}



