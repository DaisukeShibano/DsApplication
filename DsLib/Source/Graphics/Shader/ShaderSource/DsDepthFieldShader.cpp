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

			float beginDepth = 0.50;
			float endDepth = 0.80;

			vec4 depth = texture2D(depTexOri, gl_TexCoord[0].st);
			if (depth.x < beginDepth)
			{//ピンとより近い
				depth.x = (beginDepth-depth.x) / beginDepth;//０～１へ
				//depth.xyz = vec3(1.0, 0.0, 0.0);
			}
			else if (depth.x < endDepth)
			{//ピントの中
				depth.x = 0.0;
				//depth.xyz = vec3(0.0, 1.0, 0.0);
			}
			else if (0.999999 < depth.x)
			{//空とか何もない空間
				depth.x = 0.0;
			}
			else
			{//ピンとより遠い
				depth.x = (depth.x- endDepth) / (1.0-beginDepth);//０～１へ
				//depth.xyz = vec3(0.0, 0.0, 1.0);
			}

			//イーズアウト
			depth.x = depth.x - 1.0;
			depth.x = depth.x*depth.x*depth.x + 1.0;

			gl_FragColor = depth;
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
			vec4 blur = texture2D(blurTex, gl_TexCoord[0].st);
			vec4 src = texture2D(colTexEff, gl_TexCoord[0].st);
			float blend = texture2D(depTexEff, gl_TexCoord[0].st).x;

			gl_FragColor = (blend * blur) + ((1.0 - blend)*src);
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

