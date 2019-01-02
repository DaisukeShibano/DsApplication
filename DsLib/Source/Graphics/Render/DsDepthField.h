#pragma once

namespace DsLib
{
	class DsRender;
	class DsShader;
	class DsPostEffectBuffer;
}

namespace DsLib
{
	class DsDepthField
	{
	public:
		static DsDepthField* Create(const DsRender& ren, DsShader& shader, DsPostEffectBuffer& postEffectBuffer);

	public:
		virtual void DepthField()=0;

	public:
		virtual void DbgDraw() = 0;
	};


}