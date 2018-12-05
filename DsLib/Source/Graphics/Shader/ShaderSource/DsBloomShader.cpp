#include "DsPch.h"
#include "Graphics/Shader/ShaderSource/DsBloomShader.h"

using namespace DsLib;
#define DS_SHADER_STR(str) #str


namespace//高輝度抽出
{
	/***************************************************
	@brief		バーテックスシェーダー
	***************************************************/
	static const char s_vertex1[] = DS_SHADER_STR(
		void main(void)
		{
			gl_Position = ftransform();
			gl_TexCoord[0] = gl_MultiTexCoord0;
		}
	);
	/***************************************************
	@brief		フラグメントシェーダー
	***************************************************/
	static const char s_fragment1[] = DS_SHADER_STR(
		uniform sampler2D textureSource;

		void main(void)
		{
			vec3 texel = max(vec3(0.0), (texture2D(textureSource, gl_TexCoord[0].st) - 0.5).rgb);
			gl_FragColor = vec4(texel, 1.0);
		}
	);
}

//ブラーは別のシェーダー使う

namespace//合成
{
	/***************************************************
	@brief		バーテックスシェーダー
	***************************************************/
	static const char s_vertex2[] = DS_SHADER_STR(
		void main(void)
		{
			gl_Position = ftransform();
			gl_TexCoord[0] = gl_MultiTexCoord0;
		}
	);
	/***************************************************
	@brief		フラグメントシェーダー
	***************************************************/
	static const char s_fragment2[] = DS_SHADER_STR(
		uniform sampler2D textureSource;

		void main(void)
		{
			vec3 texel = max(vec3(0.0), (texture2D(textureSource, gl_TexCoord[0].st) - 0.0).rgb);
			gl_FragColor = vec4(texel, 1.0);
		}
	);
}


const char* DsLib::GetBloomVertexShader1()
{
	return s_vertex1;
}

const char* DsLib::GetBloomFragmentShader1()
{
	return s_fragment1;
}

const char* DsLib::GetBloomVertexShader2()
{
	return s_vertex2;
}

const char* DsLib::GetBloomFragmentShader2()
{
	return s_fragment2;
}

