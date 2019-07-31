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
		virtual void SetupBuffer(unsigned int tex1, unsigned int tex2, unsigned int tex3) = 0;
		virtual void CopyFrameBuffer() = 0;
		virtual void RenderFrame()=0;

		virtual void BindTexture() = 0;
		virtual void BindTextureOri() = 0;
		virtual void BindDepTextureOri() = 0;
		virtual void BindTmpColorTexture1() = 0;
		virtual void BindTmpColorTexture2() = 0;
		virtual void BindTmpColorTexture3() = 0;
		virtual void UnbindTexture() = 0;

		virtual void BindFrameBuffer() = 0;
		virtual void BindTmpFrameBuffer1() = 0;
		virtual void BindTmpFrameBuffer2() = 0;
		virtual void BindTmpFrameBuffer3() = 0;
		virtual void UnbindFrameBuffer() = 0;

		virtual void SetCurrentResult(int idx) = 0;
		virtual void BindCurrentResultTexture() = 0;

	public:
		virtual void DbgDraw() = 0;
	};


}