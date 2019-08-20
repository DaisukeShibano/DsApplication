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
			{//ssr
				m_shader.EnableShader(SHADER_TYPE::SSR1);
				//何回も計算しないでメンバで持っておく
				DsMat44f projMat = DsMat44f::Identity();
				DsMat44f modelViewMat = DsMat44f::Identity();
				glGetFloatv(GL_PROJECTION_MATRIX, projMat.mat);
				DsMat44f projInv = DsMat44f::Identity();
				DsInverseMatrix<4, float>(projMat.mat, projInv.mat);
				m_shader.SetProjectionTransform(projMat.mat);
				m_shader.SetProjectionInverseTransform(projInv.mat);

				m_shader.SetPostEffectParam(0, 1, 2);
				m_shader.NormalTex(3);
				DsGLActiveTexture(DS_GL_TEXTURE1);
				m_postEffectBuffer.BindCurrentResultTexture();//0番目
				DsGLActiveTexture(DS_GL_TEXTURE2);
				m_postEffectBuffer.BindDepTextureOri();
				DsGLActiveTexture(DS_GL_TEXTURE3);
				m_postEffectBuffer.BindNormalTexture();
				m_shader.SpecularDepthTex(4);
				DsGLActiveTexture(DS_GL_TEXTURE4);
				m_postEffectBuffer.BindSpecularDepthOri();


				m_postEffectBuffer.BindTmpFrameBuffer1();
				_Draw();
				m_postEffectBuffer.UnbindFrameBuffer();

				m_postEffectBuffer.UnbindTexture();
				DsGLActiveTexture(DS_GL_TEXTURE0);
				m_shader.DisableShader();
			}

			{//ブラー
				m_shader.EnableShader(SHADER_TYPE::BLUR_HORIZON);
				const int width = static_cast<int>(m_render.GetWidth());
				const int height = static_cast<int>(m_render.GetHeight());
				const DsShader::BlurParam blurParam = DsShader::GetBlurParam(19, 0.1f);
				m_shader.SetBlurParam(1.0f / (static_cast<float>(width)), 0, blurParam);
				DsGLActiveTexture(DS_GL_TEXTURE0);
				m_postEffectBuffer.BindTmpColorTexture1();
				m_postEffectBuffer.BindTmpFrameBuffer2();//結果を横方向バッファへ
				_Draw();
				m_postEffectBuffer.UnbindFrameBuffer();

				m_shader.EnableShader(SHADER_TYPE::BLUR_VERTICAL);
				m_shader.SetBlurParam(1.0f / static_cast<float>(height), 0, blurParam);
				DsGLActiveTexture(DS_GL_TEXTURE0);
				m_postEffectBuffer.BindTmpColorTexture2();//横方向加工済みをバインド
				m_postEffectBuffer.BindTmpFrameBuffer3();//結果を縦方向バッファへ
				_Draw();
				m_postEffectBuffer.UnbindFrameBuffer();
			}

			{//合成
				m_shader.EnableShader(SHADER_TYPE::SSR2);
				m_shader.SetPostEffectParam(0, 1, 2);
				DsGLActiveTexture(DS_GL_TEXTURE0);
				m_postEffectBuffer.BindTmpColorTexture3();//ブラー適用済み
				DsGLActiveTexture(DS_GL_TEXTURE1);
				m_postEffectBuffer.BindCurrentResultTexture();//ssr直前
				m_shader.SpecularDepthTex(4);
				DsGLActiveTexture(DS_GL_TEXTURE4);
				m_postEffectBuffer.BindSpecularDepthOri();

				m_postEffectBuffer.BindTmpFrameBuffer1();
				_Draw();
				m_postEffectBuffer.UnbindFrameBuffer();
			}

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



