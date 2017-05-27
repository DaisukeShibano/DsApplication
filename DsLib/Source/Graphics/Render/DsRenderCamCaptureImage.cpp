#include "DsPch.h"
#include "gl/glew.h"
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
		glGenRenderbuffersEXT(1, &m_rboId);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_rboId);
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_RGB, m_imageW, m_imageH);

		//フレームバッファオブジェクト生成
		glGenFramebuffersEXT(1, &m_fboId);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fboId);

		//フレームバッファとレンダーバッファを結びつける
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, m_rboId);

		//デプスバッファ。この視点用のデプスバッファがないと前後関係が正しく描画されない
		//glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fboId);//すぐ上でバインドされているので必要なし
		glGenRenderbuffersEXT(1, &m_dboId);
		glBindRenderbufferEXT(GL_RENDERBUFFER, m_dboId);
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT16, m_imageW, m_imageH);
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_dboId);

		//バインド解除
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		//glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
	}

public:
	virtual ~DsRenderImageImp()
	{
		glDeleteFramebuffersEXT(1, &m_fboId);
		glDeleteRenderbuffersEXT(1, &m_rboId);
		glDeleteRenderbuffersEXT(1, &m_dboId);
	}

public:
	virtual void BeginCapture() override
	{
		//レンダリング先を変更
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fboId);

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
		glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
		glReadPixels(
			0,                 //読み取る領域の左下隅のx座標
			0,                 //読み取る領域の左下隅のy座標 //0 or getCurrentWidth() - 1
			m_imageW,             //読み取る領域の幅
			m_imageH,            //読み取る領域の高さ
			GL_RGB,              //it means GL_BGR,           //取得したい色情報の形式
			GL_UNSIGNED_BYTE,  //読み取ったデータを保存する配列の型
			m_pImage      //ビットマップのピクセルデータ（実際にはバイト配列）へのポインタ
		);

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
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