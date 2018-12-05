#include "DsPch.h"
#include "Graphics/Shader/ShaderSource/DsBloomShader.h"

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
			gl_Position = gl_Vertex;
			gl_TexCoord[0] = gl_MultiTexCoord0;
		}
	);


	/***************************************************
	@brief		フラグメントシェーダー
	***************************************************/
	static const char s_fragment[] = DS_SHADER_STR(
		uniform sampler2D textureSource;

		void main(void)
		{
			vec3 texel = max(vec3(0.0), (texture2D(textureSource, gl_TexCoord[0].st) - 0.5).rgb);
			gl_FragColor = vec4(texel, 1.0);
		}
	);
}

const char* DsLib::GetBloomVertexShader()
{
	return s_vertex;
}

const char* DsLib::GetBloomFragmentShader()
{
	return s_fragment;
}