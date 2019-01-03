#ifndef _DS_SHADER_H_
#define _DS_SHADER_H_

namespace DsLib
{
	enum class SHADER_TYPE : int
	{
		DEFAULT,
		SHADOW_MAP,
		BLUR,
		BLOOM1,
		BLOOM2,
		SSAO,
		DEPTH_FIELD1,
		DEPTH_FIELD2,
		NUM,
	};

	class DsShader
	{
	public:
		struct BlurParam
		{
			int pixNum;
			float weight[50];
			float weightSum;
		};
	public:
		static DsShader& Create();
		static BlurParam GetBlurParam(const int pixNum);

	public:
		virtual void Initialize() = NULL;
		virtual void EnableShader( SHADER_TYPE sType ) = NULL;
		virtual void DisableShader() = NULL;
		virtual void SetTextureUnit(int unit) = NULL;
		virtual void SetUseTexture(bool isUse) = NULL;
		virtual void SetUseLight(bool isUse)=NULL;
		virtual void SetUseShadow(bool isUse)=NULL;
		virtual void SetBlurParam(DsVec2f s, int ts, const BlurParam& bp )=NULL;
		virtual void SetPostEffectParam(int effTex, int oriTex, int oriDepTex)=NULL;
		virtual void SetUseNormalMap(bool isUse) = NULL;
		virtual void SetTime(float t) = NULL;
		virtual void DepthFieldParam(int depTex, int blurTex) = NULL;

	};

}

#endif