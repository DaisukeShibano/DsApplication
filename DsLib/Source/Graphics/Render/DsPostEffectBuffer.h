#pragma once

namespace DsLib
{
	class DsRender;
	class DsShader;
}

namespace DsLib
{
	class DsPostEffectBuffer
	{
	public:
		static DsPostEffectBuffer* Create(const DsRender& ren, DsShader& shader);

	public:
		virtual void CopyFrameBuffer() = 0;
		virtual void BindTexture() = 0;
		virtual void UnbindTexture() = 0;


	public:
		virtual void DbgDraw() = 0;
	};


}