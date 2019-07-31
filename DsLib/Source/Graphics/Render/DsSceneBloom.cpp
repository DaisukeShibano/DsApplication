#include "DsPch.h"
#include "Graphics/Render/DsSceneBloom.h"
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
			{//高輝度抽出
				m_shader.EnableShader(SHADER_TYPE::BLOOM1);
				m_shader.SetPostEffectParam(0, 1, 2);
				DsGLActiveTexture(DS_GL_TEXTURE0);
				m_postEffectBuffer.BindTextureOri();//一番最初のオリジナルカラーバッファ
				m_postEffectBuffer.BindFrameBuffer();
				_Draw();
				m_postEffectBuffer.UnbindFrameBuffer();
			}
			{//ブラー
				const int width = static_cast<int>(m_render.GetWidth());
				const int height = static_cast<int>(m_render.GetHeight());
				
				m_shader.EnableShader(SHADER_TYPE::BLUR_HORIZON);
				const DsShader::BlurParam blurParam = DsShader::GetBlurParam(19);
				m_shader.SetBlurParam(1.0f / (static_cast<float>(width)), 0, blurParam);
				DsGLActiveTexture(DS_GL_TEXTURE0);
				m_postEffectBuffer.BindTexture();//0番目に輝度抽出したテクスチャをバインド
				m_postEffectBuffer.BindTmpFrameBuffer1();//結果を横方向バッファへ
				_Draw();
				m_postEffectBuffer.UnbindFrameBuffer();

				m_shader.EnableShader(SHADER_TYPE::BLUR_VERTICAL);
				m_shader.SetBlurParam(1.0f / static_cast<float>(height), 0, blurParam);
				DsGLActiveTexture(DS_GL_TEXTURE0);
				m_postEffectBuffer.BindTmpColorTexture1();//0番目に横方向加工済みをバインド
				m_postEffectBuffer.BindTmpFrameBuffer2();//結果を縦方向バッファへ
				_Draw();
				m_postEffectBuffer.UnbindFrameBuffer();
			}
			{//合成
				m_shader.EnableShader(SHADER_TYPE::BLOOM2);
				m_shader.SetPostEffectParam(0, 1, 2);
				DsGLActiveTexture(DS_GL_TEXTURE0);
				m_postEffectBuffer.BindTmpColorTexture2();//0番目に縦横方向加工済みをバインド
				DsGLActiveTexture(DS_GL_TEXTURE1);
				m_postEffectBuffer.BindTextureOri();//1番のテスクチャに加工前をバインド
				//2は使わない

				m_postEffectBuffer.BindTmpFrameBuffer1();//次のポストエフェクトへ渡す
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



