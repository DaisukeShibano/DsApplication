#ifndef _DS_SHADOWMAP_H_
#define _DS_SHADOWMAP_H_

namespace DsLib
{
	class DsRender;
	class DsShader;
}

namespace DsLib
{

	class DsShadowMap
	{
	public:
		static DsShadowMap& Create( const DsRender& ren, DsShader& pShader);

	public:
		virtual void ActiveShadow() = NULL;
		virtual void DeActiveShadow() = NULL;
		virtual void DrawDepthTex() = NULL;
	};
}

#endif