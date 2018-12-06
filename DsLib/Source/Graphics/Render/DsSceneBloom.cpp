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
			{
				m_shader.EnableShader(SHADER_TYPE::BLOOM1);
				DsGLActiveTexture(DS_GL_TEXTURE0);
				m_postEffectBuffer.BindTexture();
				_Draw();
			}
			{
				const int width = static_cast<int>(m_render.GetWidth());
				const int height = static_cast<int>(m_render.GetHeight());
				m_shader.EnableShader(SHADER_TYPE::BLUR);
				m_shader.SetBlurParam(DsVec2f(1.0f / (static_cast<float>(width)), 0.0f), 0);
				_Draw();
				m_shader.SetBlurParam(DsVec2f(0.0f, 1.0f / static_cast<float>(height)),0);
				_Draw();
			}
			{
				m_shader.EnableShader(SHADER_TYPE::BLOOM2);
				DsGLActiveTexture(DS_GL_TEXTURE1);
				m_postEffectBuffer.BindTextureOri();
				m_shader.SetPostEffectParam(1);
				_Draw();
			}
			m_shader.DisableShader();
		}

	private:
		void _Draw()
		{
			m_postEffectBuffer.BindFrameBuffer();

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

			m_postEffectBuffer.UnbindFrameBuffer();
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



