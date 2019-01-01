#ifndef _DS_RENDER_H_
#define _DS_RENDER_H_

#ifndef _DS_MODEL_RENDER_H_
#include "Graphics/Render/DsModelRender.h"
#endif

#ifndef _DS_PARTICLE_RENDER_
#include "Graphics/Render/DsParticleRender.h"
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
	class DsSceneBloom;
	class DsPostEffectBuffer;
	class DsSSAO;
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

		//	ポリゴンのみの描画
		void RendPolygon() const;

		//通常描画
		void Render( const double dt );

	public:
		const DsCamera& RefCam() const { return m_cam; }
		DsCamera& RefCam() { return m_cam; }
		DsDrawCommand& RefDrawCom() { return *m_pDrawCom; }
		const DsLight& RefLight() const { return m_light; }
		double GetWidth() const;
		double GetHeight() const;
		DsModelRender& RefAnimRender() { return m_animRender; }
		DsParticleRender& RefParticleRender() { return m_particleRender; }

	public:
		void SetEnableTexture(bool isEnable);

	public:
		//別視点でレンダリングし、画像に保存する。登録数だけレンダリング回数増えるので負荷は高い
		void RegisterCaptureImage(DsRenderCamCaptureImage* pImage);
		void UnRegisterCaptureImage(DsRenderCamCaptureImage* pImage);


	public:
		DsRenderTool& GetRenderTool(){ return *m_pRenderTool;  }

	private:
		void _RenderModel() const;

	private:
		DsCamera& m_cam;
		DsShadowMap* m_pShadow;
		DsPostEffectBuffer* m_pPostEffectBuffer;
		DsSceneBloom* m_pBloom;
		DsSSAO* m_pSSAO;
		DsShader* m_pShader;
		DsDrawCommand* m_pDrawCom;
		DsLight& m_light;
		DsRenderTool* m_pRenderTool;
		DsSys& m_sys;

	private:
		DsModelRender m_animRender;
		DsParticleRender m_particleRender;

	private:
		std::vector<DsRenderCamCaptureImage*> m_renderImages;
	};

}

#endif