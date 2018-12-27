#include "DsPch.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#ifndef _DS_RENDER_H_
#include "Graphics/Render/DsRender.h"
#endif

#include "Graphics/Camera/DsCamera.h"
#include "Graphics/Light/DsLight.h"
#include "Graphics/Light/DsLightMan.h"
#include "Graphics/Render/DsShadowMap.h"
#include "Graphics/Shader/DsShader.h"
#include "Graphics/Command/DsDrawCommand.h"
#include "Graphics/Render/Tool/DsRenderTool.h"
#include "Graphics/Render/Tool/Cloud/DsToolCloud.h"
#ifndef _DS_SYS_H_
#include "System/DsSys.h"
#endif
#ifndef _DS_WINDOW_H_
#include "System/DsWindow.h"
#endif
#ifndef _DS_RENDER_IMAGE_H_
#include "Graphics/Render/DsRenderCamCaptureImage.h"
#endif
#include "Graphics/GL/DsGLFunc.h"
#include "Graphics/Render/DsSceneBloom.h"
#include "Graphics/Render/DsPostEffectBuffer.h"

using namespace DsLib;

//static
DsRender& DsRender::Create(DsCamera& cam, DsSys& sys)
{
	DsRender* ret = new DsRender(cam, sys);
	DS_ASSERT(ret, "�������m�ێ��s");
	return *ret;
}

DsRender::DsRender(DsCamera& cam, DsSys& sys)
:m_cam(cam)
,m_pShadow(NULL)
,m_pPostEffectBuffer(NULL)
,m_pBloom(NULL)
,m_pShader(NULL)
,m_pDrawCom(NULL)
,m_light(DsLightMan::GetIns().GetSunLight())
,m_sys(sys)
,m_animRender()
,m_particleRender(cam)
,m_renderImages()
{
	if (!DsInitGLFunc()) {
		DS_ASSERT(false, "GL�֐��̏������Ɏ��s���܂����BGL�̃o�[�W������2.0�����ł���\��������܂�");
	}
	
	m_pDrawCom = &DsDrawCommand::Create(m_animRender, *this);
	
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);

	glFrontFace(GL_CCW);//�����v���肪�\
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);//���ʂ��J�����O

	// �|���S���ݒ�
	//glEnable(GL_AUTO_NORMAL);
	//glEnable(GL_NORMALIZE);

	DsLightMan::GetIns().Enable();
	m_light.Enable();

	m_pShader = &DsShader::Create();
	DS_ASSERT(m_pShader, "�������m�ێ��s");
	m_pShader->Initialize();

	m_pShadow = &DsShadowMap::Create(*this, *m_pShader);
	DS_ASSERT(m_pShadow, "�������m�ێ��s");

	m_pPostEffectBuffer = DsPostEffectBuffer::Create(*this, *m_pShader);
	DS_ASSERT(m_pPostEffectBuffer, "�������m�ێ��s");

	m_pBloom = DsSceneBloom::Create(*this, *m_pShader, *m_pPostEffectBuffer);
	DS_ASSERT(m_pBloom, "�������m�ێ��s");

	m_light.SetPos(DsVec3f::ToVec3(0, 5, 0));
	m_light.SetDir(DsVec3f::ToVec3(0, -1, 0));

	m_pRenderTool = new DsRenderTool(*m_pDrawCom, cam);
	DS_ASSERT(m_pRenderTool, "�������m�ێ��s");

	m_animRender.Initialize(*m_pShader);
}

DsRender::~DsRender()
{
	delete m_pShadow;m_pShadow=NULL;
	delete m_pPostEffectBuffer; m_pPostEffectBuffer = NULL;
	delete m_pBloom; m_pBloom = NULL;
	delete m_pShader;m_pShader=NULL;
	delete m_pDrawCom; m_pDrawCom=NULL;
}

//�d�ɉe�̑Ώۂ̕`��
void DsRender::RendPolygon() const
{
	m_animRender.RenderPolygon();
	m_pDrawCom->Exe();
}

void DsRender::Render( const double dt )
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	const DsVec3d camPos = m_cam.GetPos();
	const DsVec3d camLook = camPos + m_cam.GetRot().GetAxisZ();
	const DsVec3d camUp = m_cam.GetUp();
	gluLookAt(camPos.x, camPos.y, camPos.z,
		camLook.x, camLook.y, camLook.z,
		camUp.x, camUp.y, camUp.z);

	m_animRender.UpdateTime(dt);

	//�e�`��
	m_pShadow->DrawShadow();

	//�_�`��
	GetRenderTool().RefCloud().Update(static_cast<float>(dt));
	
	//�ʏ�V�[���`��p�V�F�[�_�[
	m_pShader->EnableShader(SHADER_TYPE::DEFAULT);

	//�ʏ�V�[���`��
	_RenderModel();
	
	//�O���Ŏg���p�̃����_�����O�摜�̕ۑ�
	for (DsRenderCamCaptureImage* pImage : m_renderImages) {
		pImage->BeginCapture();
		_RenderModel();
		pImage->EndCapture();
	}

	//���̎��_�̃t���[���o�b�t�@���|�X�g�v���Z�X�p�o�b�t�@�ɃR�s�[
	m_pPostEffectBuffer->CopyFrameBuffer();

	//�u���[��
	m_pBloom->Bloom();
	
	//�|�X�g�G�t�F�N�g�������_�����O��ʂ�
	m_pPostEffectBuffer->RenderFrame();

	m_pShader->DisableShader();
	
	//�f�o�b�O�p
	//m_pShadow->DrawDepthTex();
}

void DsRender::Update( const double dt )
{
	DsLightMan::GetIns().Update();
	Render(dt);
	m_pDrawCom->Clear();
}

double DsRender::GetWidth() const
{
	return static_cast<double>(m_sys.RefWindow().GetCurrentWindowW());
}

double DsRender::GetHeight() const
{
	return static_cast<double>(m_sys.RefWindow().GetCurrentWindowH());
}

void DsRender::SetEnableTexture(bool isEnable)
{
	m_pShader->SetUseTexture(isEnable);
}

void DsRender::RegisterCaptureImage(DsRenderCamCaptureImage* pImage)
{
	if (pImage) {
		m_renderImages.push_back(pImage);
	}
}

void DsRender::UnRegisterCaptureImage(DsRenderCamCaptureImage* pImage)
{
	if (pImage) {
		const size_t size = m_renderImages.size();
		for (size_t i=0; i< size ; ++i){
			if (m_renderImages[i] == pImage) {
				m_renderImages[i] = NULL;
				break;
			}
		}
	}
}

void DsRender::_RenderModel() const
{
	DsLightMan::GetIns().Enable();
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//���C���[�t���[�������ʂ��𒅑ւ�����
	//DsDbgSys::GetIns().RefDrawCom().DrawSphere(m_light.GetPos(), 0.1f);
	m_pShader->SetUseLight(true);
	m_pShader->SetUseTexture(true);
	m_animRender.Render();
	m_particleRender.Render();

	m_pShader->SetUseTexture(false);
	m_animRender.RenderNonMaterial();

	m_pShader->SetUseLight(false);
	m_pDrawCom->Exe();
}

