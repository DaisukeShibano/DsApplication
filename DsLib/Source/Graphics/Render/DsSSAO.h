#pragma once

namespace DsLib
{
	class DsRender;
	class DsShader;
	class DsPostEffectBuffer;
}

namespace DsLib
{
	class DsSSAO
	{
	public:
		static DsSSAO* Create(const DsRender& ren, DsShader& shader, DsPostEffectBuffer& postEffectBuffer);

	public:
		virtual void SSAO()=0;

	public:
		virtual void DbgDraw() = 0;
	};


}