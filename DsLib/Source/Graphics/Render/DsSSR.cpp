#include "DsPch.h"
#include "Graphics/Render/DsSSR.h"
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
	class DsSSRImp : public DsSSR
	{
	public:
		DsSSRImp(const DsRender& ren, DsShader& shader, DsPostEffectBuffer& postEffectBuffer)
			: m_render(ren)
			, m_shader(shader)
			, m_postEffectBuffer(postEffectBuffer)
		{
		}

		virtual void SSR() override
		{
			m_shader.EnableShader(SHADER_TYPE::SSAO);
			m_shader.SetPostEffectParam(0, 1, 2);
			DsGLActiveTexture(DS_GL_TEXTURE0);
			m_postEffectBuffer.BindCurrentResultTexture();//一つ前のポストエフェクトの結果をもらう
			DsGLActiveTexture(DS_GL_TEXTURE2);
			m_postEffectBuffer.BindDepTextureOri();
			m_postEffectBuffer.BindTmpFrameBuffer1();
			_Draw();
			m_postEffectBuffer.UnbindFrameBuffer();
			m_postEffectBuffer.UnbindTexture();
			DsGLActiveTexture(DS_GL_TEXTURE0);
			m_shader.DisableShader();

			m_postEffectBuffer.SetCurrentResult(1);//1番目を結果に
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
DsSSR* DsSSR::Create(const DsRender& ren, DsShader& shader, DsPostEffectBuffer& postEffectBuffer)
{
	return new DsSSRImp(ren, shader, postEffectBuffer);
}



