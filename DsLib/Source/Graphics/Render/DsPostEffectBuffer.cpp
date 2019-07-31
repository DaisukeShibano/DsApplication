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
	static const int _TEMP_TEX_NUM = 2;

	static const GLenum s_bufEnum[] = {
		DS_GL_COLOR_ATTACHMENT0, //   加工用カラーバッファ
		DS_GL_COLOR_ATTACHMENT1, //   元画像カラーバッファ
		DS_GL_COLOR_ATTACHMENT2, //   法線バッファ
	};

	class DsPostEffectBufferImp : public DsPostEffectBuffer
	{
	public:
		DsPostEffectBufferImp(const DsRender& ren, DsShader& shader)
			: m_render(ren)
			, m_shader(shader)
			, m_fboId(0)
			, m_fboPostId(0)
			, m_texId(0)
			, m_texOriId(0)
			, m_depTexOriId(0)
			, m_normalOriId(0)
			, m_tempFbo{}
			, m_tempColorTex{}
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
			glBindTexture(GL_TEXTURE_2D, 0);
			
			//カラーテクスチャ
			glGenTextures(1, &m_texOriId);
			glBindTexture(GL_TEXTURE_2D, m_texOriId);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexImage2D(GL_TEXTURE_2D, 0, DS_GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, 0);
			glBindTexture(GL_TEXTURE_2D, 0);

			// デプス値テクスチャ
			glGenTextures(1, &m_depTexOriId);
			glBindTexture(GL_TEXTURE_2D, m_depTexOriId);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
			glBindTexture(GL_TEXTURE_2D, 0);

			//法線テクスチャ
			glGenTextures(1, &m_normalOriId);
			glBindTexture(GL_TEXTURE_2D, m_normalOriId);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexImage2D(GL_TEXTURE_2D, 0, DS_GL_RGB32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
			glBindTexture(GL_TEXTURE_2D, 0);

			//シーンレンダリングの情報保存用フレームバッファオブジェクト
			DsGLGenFramebuffers(1, &m_fboId);
			DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, m_fboId);
			DsGLFramebufferTexture2D(DS_GL_FRAMEBUFFER, s_bufEnum[0], GL_TEXTURE_2D, m_texId, 0);
			DsGLFramebufferTexture2D(DS_GL_FRAMEBUFFER, s_bufEnum[1], GL_TEXTURE_2D, m_texOriId, 0);
			DsGLFramebufferTexture2D(DS_GL_FRAMEBUFFER, s_bufEnum[2], GL_TEXTURE_2D, m_normalOriId, 0);
			DsGLFramebufferTexture2D(DS_GL_FRAMEBUFFER, DS_GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depTexOriId, 0);
			
			//ポストエフェクト用fbo
			DsGLGenFramebuffers(1, &m_fboPostId);
			DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, m_fboPostId);
			DsGLFramebufferTexture2D(DS_GL_FRAMEBUFFER, DS_GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texId, 0);

			//一時計算用テクスチャとfbo
			glGenTextures(2, m_tempColorTex);
			for (int i = 0; i < _TEMP_TEX_NUM; ++i) {
				glBindTexture(GL_TEXTURE_2D, m_tempColorTex[i]);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				glTexImage2D(GL_TEXTURE_2D, 0, DS_GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, 0);
			}
			DsGLGenFramebuffers(2, m_tempFbo);
			for (int i = 0; i < _TEMP_TEX_NUM; ++i) {
				DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, m_tempFbo[i]);
				DsGLFramebufferTexture2D(DS_GL_FRAMEBUFFER, DS_GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tempColorTex[i], 0);
			}

			DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, 0);
			
		}

		virtual ~DsPostEffectBufferImp()
		{
			glDeleteTextures(1, &m_texId);
			DsGLDeleteFramebuffers(1, &m_fboId);
			DsGLDeleteFramebuffers(1, &m_fboPostId);
			glDeleteTextures(1, &m_texOriId);
			glDeleteTextures(1, &m_depTexOriId);
			glDeleteTextures(1, &m_normalOriId);
			DsGLDeleteFramebuffers(2, m_tempFbo);
			glDeleteTextures(2, m_tempColorTex);
		}

		virtual void SetupBuffer(unsigned int tex1, unsigned int tex2, unsigned int tex3) override
		{
			DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, m_fboId);

			DsGLActiveTexture(tex1);
			glBindTexture(GL_TEXTURE_2D, m_texId);
			DsGLActiveTexture(tex2);
			glBindTexture(GL_TEXTURE_2D, m_texOriId);
			DsGLActiveTexture(tex3);
			glBindTexture(GL_TEXTURE_2D, m_normalOriId);
			DsGLActiveTexture(DS_GL_TEXTURE0);

			// レンダーターゲットを指定する
			// フラグメントシェーダー内でgl_FragData[]で対応するバッファに書き出せる
			DsGLDrawBuffers(sizeof(s_bufEnum) / sizeof(s_bufEnum[0]), s_bufEnum);

			//バッファ切り替えたので再クリア
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		virtual void CopyFrameBuffer() override
		{
			//バインドし直し
			//シェーダー内で参照する番号に合わせる
			//毎回バインドしなおしてるのでいらない
			//DsGLActiveTexture(DS_GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, m_texId);
			//DsGLActiveTexture(DS_GL_TEXTURE1);
			//glBindTexture(GL_TEXTURE_2D, m_texOriId);
			//DsGLActiveTexture(DS_GL_TEXTURE2);
			//glBindTexture(GL_TEXTURE_2D, m_depTexOriId);
			//DsGLActiveTexture(DS_GL_TEXTURE3);
			//glBindTexture(GL_TEXTURE_2D, m_normalOriId);
			//DsGLActiveTexture(DS_GL_TEXTURE0);

			DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, 0);

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

		virtual void BindTextureOri() override
		{
			glBindTexture(GL_TEXTURE_2D, m_texOriId);
		}

		virtual void BindDepTextureOri() override
		{
			glBindTexture(GL_TEXTURE_2D, m_depTexOriId);
		}

		//ここには直前のポストエフェクトの結果が入るように組む
		//計算途中のテンポラリとして使っても良い
		virtual void BindTmpColorTexture1()override
		{
			glBindTexture(GL_TEXTURE_2D, m_tempColorTex[0]);
		}

		//好きにつかっていい奴
		virtual void BindTmpColorTexture2()override
		{
			glBindTexture(GL_TEXTURE_2D, m_tempColorTex[1]);
		}

		virtual void UnbindTexture() override
		{
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		virtual void BindFrameBuffer() override
		{
			DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, m_fboPostId);
		}

		virtual void BindTmpFrameBuffer1()override
		{
			DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, m_tempFbo[0]);
		}

		virtual void BindTmpFrameBuffer2()override
		{
			DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, m_tempFbo[1]);
		}

		virtual void UnbindFrameBuffer() override
		{
			DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, 0);
		}

		
		virtual void CopyTmpColorTexture1() override
		{
			const int width = static_cast<int>(m_render.GetWidth());
			const int height = static_cast<int>(m_render.GetHeight());
			glCopyTexSubImage2D(m_tempColorTex[0], 0, 0, 0, 0, 0, width, height);
		}

		virtual void CopyTmpColorTexture2() override
		{
			const int width = static_cast<int>(m_render.GetWidth());
			const int height = static_cast<int>(m_render.GetHeight());
			glCopyTexSubImage2D(m_tempColorTex[1], 0, 0, 0, 0, 0, width, height);
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
		GLuint m_fboPostId;
		GLuint m_texId;
		GLuint m_texOriId;
		GLuint m_depTexOriId;
		GLuint m_normalOriId;
		GLuint m_tempFbo[_TEMP_TEX_NUM];
		GLuint m_tempColorTex[_TEMP_TEX_NUM];
	};
}

//static
DsPostEffectBuffer* DsPostEffectBuffer::Create(const DsRender& ren, DsShader& shader)
{
	return new DsPostEffectBufferImp(ren, shader);
}