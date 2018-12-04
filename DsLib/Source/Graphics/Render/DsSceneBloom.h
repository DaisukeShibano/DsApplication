#pragma once

namespace DsLib
{
	class DsRender;
	class DsShader;
}

namespace DsLib
{
	class DsSceneBloom
	{
	public:
		static DsSceneBloom* Create(const DsRender& ren, DsShader& shader);

	public:
		virtual void Bloom()=0;

	public:
		virtual void DbgDraw() = 0;
	};


}