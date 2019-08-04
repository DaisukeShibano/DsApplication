#include "DsPch.h"
#include "Graphics/Shader/ShaderSource/DsSSRShader.h"

using namespace DsLib;
#define DS_SHADER_STR(str) #str


namespace
{
	/***************************************************
	@brief		バーテックスシェーダー
	***************************************************/
	static const char s_vertex[] = DS_SHADER_STR(
		void main(void)
		{
			gl_Position = ftransform();
			gl_TexCoord[0] = gl_MultiTexCoord0;
		}
	);
	/***************************************************
	@brief		フラグメントシェーダー
	***************************************************/
	static const char s_fragment[] = DS_SHADER_STR(
		uniform sampler2D colTexEff;
		uniform sampler2D depTexOri;
		uniform sampler2D normalTex;

		void main(void)
		{
			vec4 srcCol = texture2D(colTexEff, gl_TexCoord[0].st);
			gl_FragColor = srcCol;
		}
	);
}

const char* DsLib::GetSSRVertexShader()
{
	return s_vertex;
}

const char* DsLib::GetSSRFragmentShader()
{
	return s_fragment;
}
