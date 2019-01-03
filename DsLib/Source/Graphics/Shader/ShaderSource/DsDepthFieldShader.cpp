#include "DsPch.h"
#include "Graphics/Shader/ShaderSource/DsDepthFieldShader.h"

using namespace DsLib;
#define DS_SHADER_STR(str) #str


namespace//デプス値算出
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
		uniform sampler2D depTexOri;

		void main(void)
		{
			//ここで求めた深度値がぼやけた画像とのブレンド率になる
			//ピント付近は0にしてそこから遠いほど高くする

			gl_FragColor = texture2D(depTexOri, gl_TexCoord[0].st);
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
		uniform sampler2D colTexEff;
		uniform sampler2D depTexEff;
		uniform sampler2D blurTex;

		void main(void)
		{
			gl_FragColor = texture2D(colTexEff, gl_TexCoord[0].st);
		}
	);
}


const char* DsLib::GetDepthFieldVertexShader1()
{
	return s_vertex1;
}

const char* DsLib::GetDepthFieldFragmentShader1()
{
	return s_fragment1;
}

const char* DsLib::GetDepthFieldVertexShader2()
{
	return s_vertex2;
}

const char* DsLib::GetDepthFieldFragmentShader2()
{
	return s_fragment2;
}

