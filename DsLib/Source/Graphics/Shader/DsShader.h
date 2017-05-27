#ifndef _DS_SHADER_H_
#define _DS_SHADER_H_

namespace DsLib
{
	enum class SHADER_TYPE : int
	{
		DEFAULT,
		SHADOW_MAP,
		NUM,
	};

	class DsShader
	{
	public:
		static DsShader& Create();

	public:
		virtual void Initialize() = NULL;
		virtual void EnableShader( SHADER_TYPE sType ) = NULL;
		virtual void DisableShader() = NULL;
		virtual void SetTextureUnit(int unit) = NULL;
		virtual void SetUseTexture(bool isUse) = NULL;
		virtual void SetUseLight(bool isUse)=NULL;
		virtual void SetUseShadow(bool isUse)=0;
	};

}

#endif