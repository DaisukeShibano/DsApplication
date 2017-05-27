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
	//2��n��ɂ���
	for (m_imageW = 2; m_imageW < imageW; m_imageW *= 2);
	for (m_imageH = 2; m_imageH < imageW; m_imageH *= 2);

	m_imageSize = m_imageW*m_imageH*3;
	if (m_imageSize > 0) {
		m_pImage = new unsigned char[m_imageSize];
	}
	DS_ASSERT(m_pImage, "�������m�ێ��s");
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
		//�����_�[�o�b�t�@�I�u�W�F�N�g����
		glGenRenderbuffersEXT(1, &m_rboId);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_rboId);
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_RGB, m_imageW, m_imageH);

		//�t���[���o�b�t�@�I�u�W�F�N�g����
		glGenFramebuffersEXT(1, &m_fboId);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fboId);

		//�t���[���o�b�t�@�ƃ����_�[�o�b�t�@�����т���
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, m_rboId);

		//�f�v�X�o�b�t�@�B���̎��_�p�̃f�v�X�o�b�t�@���Ȃ��ƑO��֌W���������`�悳��Ȃ�
		//glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fboId);//������Ńo�C���h����Ă���̂ŕK�v�Ȃ�
		glGenRenderbuffersEXT(1, &m_dboId);
		glBindRenderbufferEXT(GL_RENDERBUFFER, m_dboId);
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT16, m_imageW, m_imageH);
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_dboId);

		//�o�C���h����
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
		//�����_�����O���ύX
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fboId);

		//�J�����̎��_�ɂ���

		// ���̃r���[�|�[�g������Ŗ߂����߂Ɋm��
		glGetIntegerv(GL_VIEWPORT, m_srcViewport);
		// �r���[�|�[�g���J�����̑傫���ɕύX
		glViewport(0, 0, m_imageW, m_imageH);
		
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();	// ���̃v���W�F�N�V�����s���ޔ������Ă���
		//// �����v���W�F�N�V�����s��̐����Ǝ擾
		glLoadIdentity();
		gluPerspective(m_cam.GetFovy(), (double)m_imageW / (double)m_imageH, m_cam.GetNear(), m_cam.GetFar());
		
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();	// ���̃��f���r���[�s���ޔ������Ă���
		//// �J���[�C�f�v�X�o�b�t�@�̃N���A
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
		// �ޔ������Ă��������_�s������ɖ߂�
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		// ���̃r���[�|�[�g�s��ɖ߂�
		glViewport(m_srcViewport[0], m_srcViewport[1], m_srcViewport[2], m_srcViewport[3]);

		//�摜�擾
		glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
		glReadPixels(
			0,                 //�ǂݎ��̈�̍�������x���W
			0,                 //�ǂݎ��̈�̍�������y���W //0 or getCurrentWidth() - 1
			m_imageW,             //�ǂݎ��̈�̕�
			m_imageH,            //�ǂݎ��̈�̍���
			GL_RGB,              //it means GL_BGR,           //�擾�������F���̌`��
			GL_UNSIGNED_BYTE,  //�ǂݎ�����f�[�^��ۑ�����z��̌^
			m_pImage      //�r�b�g�}�b�v�̃s�N�Z���f�[�^�i���ۂɂ̓o�C�g�z��j�ւ̃|�C���^
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