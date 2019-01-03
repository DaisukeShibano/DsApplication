#include "DsPch.h"
#ifndef _DS_BLUR_SHADER_H_
#include "Graphics/Shader/ShaderSource/DsBlurShader.h"
#endif

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
		uniform vec2 ScaleU;
		uniform sampler2D textureSource;

		//パラメータ
		const int maxPixNum = 50;
		uniform int pixNum;
		uniform float weight[maxPixNum];
		uniform float weightSum;

		void main(void)
		{
			vec4 color = vec4(0.0);
			int minPix = pixNum / 2;
			for (int i = 0; i < pixNum; ++i)
			{
				float pix = float(i -minPix);
				color += texture2D(textureSource, gl_TexCoord[0].st + vec2(pix*ScaleU.x, pix*ScaleU.y))*weight[i] / weightSum;
			}

			gl_FragColor = color;
		}
	);
}

const char* DsLib::GetBlurVertexShader()
{
	return s_vertex;
}

const char* DsLib::GetBlurFragmentShader()
{
	return s_fragment;
}