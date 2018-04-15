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

using namespace DsLib;

//static
DsRender& DsRender::Create(DsCamera& cam, DsSys& sys)
{
	DsRender* ret = new DsRender(cam, sys);
	DS_ASSERT(ret, "ƒƒ‚ƒŠŠm•ÛŽ¸”s");
	return *ret;
}

DsRender::DsRender(DsCamera& cam, DsSys& sys)
:m_cam(cam)
,m_pShadow(NULL)
,m_pShader(NULL)
,m_pDrawCom(NULL)
,m_light(DsLightMan::GetIns().GetSunLight())
,m_sys(sys)
,m_animRender()
,m_particleRender(cam)
,m_renderImages()
{
	
	m_pDrawCom = &DsDrawCommand::Create(m_animRender, *this);
	
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);

	glFrontFace(GL_CCW);//”½ŽžŒvŽü‚è‚ª•\
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);//— –Ê‚ðƒJƒŠƒ“ƒO

	// ƒ|ƒŠƒSƒ“Ý’è
	//glEnable(GL_AUTO_NORMAL);
	//glEnable(GL_NORMALIZE);

	DsLightMan::GetIns().Enable();
	m_light.Enable();

	m_pShader = &DsShader::Create();
	DS_ASSERT(m_pShader, "ƒƒ‚ƒŠŠm•ÛŽ¸”s");
	
	m_pShadow = &DsShadowMap::Create(*this, *m_pShader);
	DS_ASSERT(m_pShadow, "ƒƒ‚ƒŠŠm•ÛŽ¸”s");

	m_pShader->Initialize();

	m_light.SetPos(DsVec3f::ToVec3(0, 5, 0));
	m_light.SetDir(DsVec3f::ToVec3(0, -1, 0));

	m_pRenderTool = new DsRenderTool(*m_pDrawCom, cam);
	DS_ASSERT(m_pRenderTool, "ƒƒ‚ƒŠŠm•ÛŽ¸”s");
}

DsRender::~DsRender()
{
	delete m_pShadow;m_pShadow=NULL;
	delete m_pShader;m_pShader=NULL;
	delete m_pDrawCom; m_pDrawCom=NULL;
}

//d‚É‰e‚Ì‘ÎÛ‚Ì•`‰æ
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

	//‰e•`‰æ
	m_pShadow->DrawShadow();

	//‰_•`‰æ
	GetRenderTool().RefCloud().Update(static_cast<float>(dt));
	
	//ƒV[ƒ“•`‰æ
	m_pShader->EnableShader(SHADER_TYPE::DEFAULT);
	
	_RenderModel();
	//ŠO•”‚ÅŽg‚¤—p‚ÌƒŒƒ“ƒ_ƒŠƒ“ƒO‰æ‘œ‚Ì•Û‘¶
	for (DsRenderCamCaptureImage* pImage : m_renderImages) {
		pImage->BeginCapture();
		_RenderModel();
		pImage->EndCapture();
	}
	
	m_pShader->DisableShader();
	
	//ƒfƒoƒbƒO—p
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
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//ƒƒCƒ„[ƒtƒŒ[ƒ€‚©•’Ê‚©‚ð’…‘Ö‚¦‚ç‚ê‚é
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

