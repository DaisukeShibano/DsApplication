#pragma once

namespace DsLib
{
	class DsRender;
	class DsShader;
	class DsPostEffectBuffer;
}

namespace DsLib
{
	class DsSSR
	{
	public:
		static DsSSR* Create(const DsRender& ren, DsShader& shader, DsPostEffectBuffer& postEffectBuffer);

	public:
		virtual void SSR()=0;

	public:
		virtual void DbgDraw() = 0;
	};


}