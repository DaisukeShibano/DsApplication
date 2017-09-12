#include "DsPch.h"
#ifndef _DS_GL_FUNC_
#include "Graphics/GL/DsGLFunc.h"
#endif
#include <gl/GL.h>
#include <gl/GLU.h>
#ifndef _DS_RENDER_CAM_CAPTURE_IMAGE_H_
#include "Graphics/Render/DsRenderCamCaptureImage.h"
#endif

#include "Graphics/Camera/DsCamera.h"

using namespace DsLib;

DsRenderCamCaptureImage::DsRenderCamCaptureImage(const DsCamera& cam, int imageW, int imageH)
	: m_cam(cam)
	, m_pImage(NULL)
	, m_imageSize(0)
	, m_imageW(0)
	, m_imageH(0)
{
	//2のn乗にする
	for (m_imageW = 2; m_imageW < imageW; m_imageW *= 2);
	for (m_imageH = 2; m_imageH < imageW; m_imageH *= 2);

	m_imageSize = m_imageW*m_imageH*3;
	if (m_imageSize > 0) {
		m_pImage = new unsigned char[m_imageSize];
	}
	DS_ASSERT(m_pImage, "メモリ確保失敗");
	memset(m_pImage, 0, sizeof(unsigned char)*m_imageSize);
}
DsRenderCamCaptureImage::~DsRenderCamCaptureImage()
{
	delete m_pImage;
	m_pImage = NULL;
}

class DsRenderImageImp : public DsRenderCamCaptureImage
{
public:
	DsRenderImageImp(const DsCamera& cam, int imageW, int imageH)
		: DsRenderCamCaptureImage(cam, imageW, imageH)
		, m_fboId(0)
		, m_rboId(0)
		, m_dboId(0)
		//, m_srcViewport()
	{
		//レンダーバッファオブジェクト生成
		DsGLGenRenderbuffers(1, &m_rboId);
		DsGLBindRenderbuffer(DS_GL_RENDERBUFFER, m_rboId);
		DsGLRenderbufferStorage(DS_GL_RENDERBUFFER, GL_RGB, m_imageW, m_imageH);

		//フレームバッファオブジェクト生成
		DsGLGenFramebuffers(1, &m_fboId);
		DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, m_fboId);

		//フレームバッファとレンダーバッファを結びつける
		DsGLFramebufferRenderbuffer(DS_GL_FRAMEBUFFER, DS_GL_COLOR_ATTACHMENT0, DS_GL_RENDERBUFFER, m_rboId);

		//デプスバッファ。この視点用のデプスバッファがないと前後関係が正しく描画されない
		//DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, m_fboId);//すぐ上でバインドされているので必要なし
		DsGLGenRenderbuffers(1, &m_dboId);
		DsGLBindRenderbuffer(DS_GL_RENDERBUFFER, m_dboId);
		DsGLRenderbufferStorage(DS_GL_RENDERBUFFER, DS_GL_DEPTH_COMPONENT16, m_imageW, m_imageH);
		DsGLFramebufferRenderbuffer(DS_GL_FRAMEBUFFER, DS_GL_DEPTH_ATTACHMENT, DS_GL_RENDERBUFFER, m_dboId);

		//バインド解除
		DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, 0);
		//DsGLBindRenderbuffer(DS_GL_RENDERBUFFER, 0);
	}

public:
	virtual ~DsRenderImageImp()
	{
		DsGLDeleteFramebuffers(1, &m_fboId);
		DsGLDeleteRenderbuffers(1, &m_rboId);
		DsGLDeleteRenderbuffers(1, &m_dboId);
	}

public:
	virtual void BeginCapture() override
	{
		//レンダリング先を変更
		DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, m_fboId);

		//カメラの視点にする

		// 今のビューポート情報を後で戻すために確保
		glGetIntegerv(GL_VIEWPORT, m_srcViewport);
		// ビューポートをカメラの大きさに変更
		glViewport(0, 0, m_imageW, m_imageH);
		
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();	// 今のプロジェクション行列を退避させておく
		//// 光源プロジェクション行列の生成と取得
		glLoadIdentity();
		gluPerspective(m_cam.GetFovy(), (double)m_imageW / (double)m_imageH, m_cam.GetNear(), m_cam.GetFar());
		
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();	// 今のモデルビュー行列を退避させておく
		//// カラー，デプスバッファのクリア
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearDepth(1.0f);
		glLoadIdentity();
		const DsVec3f camPos = m_cam.GetPos();
		const DsVec3f camLook = camPos + m_cam.GetRot().GetAxisZ();
		const DsVec3f camUp = m_cam.GetUp();
		gluLookAt(camPos.x, camPos.y, camPos.z,
			camLook.x, camLook.y, camLook.z,
			camUp.x, camUp.y, camUp.z);
	}

	virtual void EndCapture() override
	{
		// 退避させておいた視点行列を元に戻す
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		// 元のビューポート行列に戻す
		glViewport(m_srcViewport[0], m_srcViewport[1], m_srcViewport[2], m_srcViewport[3]);

		//画像取得
		glReadBuffer(DS_GL_COLOR_ATTACHMENT0);
		glReadPixels(
			0,                 //読み取る領域の左下隅のx座標
			0,                 //読み取る領域の左下隅のy座標 //0 or getCurrentWidth() - 1
			m_imageW,             //読み取る領域の幅
			m_imageH,            //読み取る領域の高さ
			GL_RGB,              //it means GL_BGR,           //取得したい色情報の形式
			GL_UNSIGNED_BYTE,  //読み取ったデータを保存する配列の型
			m_pImage      //ビットマップのピクセルデータ（実際にはバイト配列）へのポインタ
		);

		DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, 0);
	}

private:
	GLuint m_fboId;
	GLuint m_rboId;
	GLuint m_dboId;
	GLint m_srcViewport[4];
};

//static 
DsRenderCamCaptureImage* DsRenderCamCaptureImage::Create(const DsCamera& cam, int imageW, int imageH)
{
	return new DsRenderImageImp(cam, imageW, imageH);
}