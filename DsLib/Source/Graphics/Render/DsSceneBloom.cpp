#include "DsPch.h"
#include "Graphics/Render/DsSceneBloom.h"
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
	class DsSceneBloomImp : public DsSceneBloom
	{
	public:
		DsSceneBloomImp(const DsRender& ren, DsShader& shader, DsPostEffectBuffer& postEffectBuffer)
			: m_render(ren)
			, m_shader(shader)
			, m_postEffectBuffer(postEffectBuffer)
		{
			
		}

		virtual void Bloom() override
		{
			m_shader.EnableShader(SHADER_TYPE::BLOOM);

			m_postEffectBuffer.BindFrameBuffer();
				
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			glOrtho(0, 1, 0, 1, 0, 1);
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();

			DsGLActiveTexture(DS_GL_TEXTURE0);
			
			m_postEffectBuffer.BindTexture();
			glBegin(GL_QUADS);
			glTexCoord2d(0, 0); glVertex3d(0, 0, 0);
			glTexCoord2d(1, 0); glVertex3d(1, 0, 0);
			glTexCoord2d(1, 1); glVertex3d(1, 1, 0);
			glTexCoord2d(0, 1); glVertex3d(0, 1, 0);
			glEnd();
			m_postEffectBuffer.UnbindTexture();

			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();

			m_postEffectBuffer.UnbindFrameBuffer();

			m_shader.DisableShader();
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



