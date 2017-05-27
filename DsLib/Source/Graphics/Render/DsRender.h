#ifndef _DS_RENDER_H_
#define _DS_RENDER_H_

#ifndef _DS_ANIM_MODEL_RENDER_H_
#include "Graphics/Render/DsAnimModelRender.h"
#endif

namespace DsLib
{
	class DsCamera;
	class DsLight;
	class DsShadowMap;
	class DsShader;
	class DsDrawCommand;
	class DsRenderTool;
	class DsSys;
	class DsRenderCamCaptureImage;
}

namespace DsLib
{
	class DsRender
	{
	public:
		static DsRender& Create(DsCamera& cam, DsSys& sys);

	private:
		DsRender(DsCamera& cam, DsSys& sys);

	public:
		virtual ~DsRender();

	public:
		void Update( const double dt );

		//	�|���S���݂̂̕`��
		void RendPolygon() const;

		//�ʏ�`��
		void Render( const float dt );

	public:
		const DsCamera& RefCam() const { return m_cam; }
		DsCamera& RefCam() { return m_cam; }
		DsDrawCommand& RefDrawCom() { return *m_pDrawCom; }
		const DsLight& RefLight() const { return m_light; }
		float GetWidth() const;
		float GetHeight() const;
		DsAnimModelRender& RefAnimRender() { return m_animRender; }

	public:
		void SetEnableTexture(bool isEnable);

	public:
		//�ʎ��_�Ń����_�����O���A�摜�ɕۑ�����B�o�^�����������_�����O�񐔑�����̂ŕ��ׂ͍���
		void RegisterCaptureImage(DsRenderCamCaptureImage* pImage);
		void UnRegisterCaptureImage(DsRenderCamCaptureImage* pImage);


	public:
		DsRenderTool& GetRenderTool(){ return *m_pRenderTool;  }

	private:
		void _RenderModel() const;

	private:
		DsCamera& m_cam;
		DsShadowMap* m_pShadow;
		DsShader* m_pShader;
		DsDrawCommand* m_pDrawCom;
		DsLight& m_light;
		DsRenderTool* m_pRenderTool;
		DsSys& m_sys;

	private:
		DsAnimModelRender m_animRender;

	private:
		std::vector<DsRenderCamCaptureImage*> m_renderImages;
	};

}

#endif