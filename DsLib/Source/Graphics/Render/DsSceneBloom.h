#pragma once

namespace DsLib
{
	class DsRender;
	class DsShader;
	class DsPostEffectBuffer;
}

namespace DsLib
{
	class DsSceneBloom
	{
	public:
		static DsSceneBloom* Create(const DsRender& ren, DsShader& shader, DsPostEffectBuffer& postEffectBuffer);

	public:
		virtual void Bloom()=0;

	public:
		virtual void DbgDraw() = 0;
	};


}