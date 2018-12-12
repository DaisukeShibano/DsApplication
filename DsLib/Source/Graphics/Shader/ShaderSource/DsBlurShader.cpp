#include "DsPch.h"
#ifndef _DS_BLUR_SHADER_H_
#include "Graphics/Shader/ShaderSource/DsBlurShader.h"
#endif

using namespace DsLib;
#define DS_SHADER_STR(str) #str

namespace
{
	/***************************************************
	@brief		�o�[�e�b�N�X�V�F�[�_�[
	***************************************************/
	static const char s_vertex[] = DS_SHADER_STR(
		void main(void)
		{
			gl_Position = ftransform();
			gl_TexCoord[0] = gl_MultiTexCoord0;
		}
	);


	/***************************************************
	@brief		�t���O�����g�V�F�[�_�[
	***************************************************/
	static const char s_fragment[] = DS_SHADER_STR(
		uniform vec2 ScaleU;
		uniform sampler2D textureSource;

		// Portability prevented us from using a const array of vec2
		// Mac shader compiler don't support it.
		
		//const vec2 gaussFilter[7] =
		//{
		//	-3.0,	0.015625,
		//	-2.0,	0.09375,
		//	-1.0,	0.234375,
		//	0.0,	0.3125,
		//	1.0,	0.234375,
		//	2.0,	0.09375,
		//	3.0,	0.015625
		//};
		

		void main(void)
		{
			vec4 color = vec4(0.0);
			//for( int i = 0; i < 7; i++ )
			//{
			//	color += texture2D( textureSource, gl_TexCoord[0].st + vec2( gaussFilter[i].x*ScaleU.x, gaussFilter[i].x*ScaleU.y ) )*gaussFilter[i].y;
			//}
			
			//7*7
			//color += texture2D(textureSource, gl_TexCoord[0].st + vec2(-3.0*ScaleU.x, -3.0*ScaleU.y)) * 0.015625;
			//color += texture2D(textureSource, gl_TexCoord[0].st + vec2(-2.0*ScaleU.x, -2.0*ScaleU.y))*0.09375;
			//color += texture2D(textureSource, gl_TexCoord[0].st + vec2(-1.0*ScaleU.x, -1.0*ScaleU.y))*0.234375;
			//color += texture2D(textureSource, gl_TexCoord[0].st + vec2(0.0, 0.0))*0.3125;
			//color += texture2D(textureSource, gl_TexCoord[0].st + vec2(1.0*ScaleU.x, 1.0*ScaleU.y))*0.234375;
			//color += texture2D(textureSource, gl_TexCoord[0].st + vec2(2.0*ScaleU.x, 2.0*ScaleU.y))*0.09375;
			//color += texture2D(textureSource, gl_TexCoord[0].st + vec2(3.0*ScaleU.x, 3.0*ScaleU.y)) * 0.015625;

			//5*5
			//color += texture2D(textureSource, gl_TexCoord[0].st + vec2(-2.0*ScaleU.x, -2.0*ScaleU.y))*0.0625;
			//color += texture2D(textureSource, gl_TexCoord[0].st + vec2(-1.0*ScaleU.x, -1.0*ScaleU.y))*0.25;
			//color += texture2D(textureSource, gl_TexCoord[0].st + vec2(0.0, 0.0))*0.375;
			//color += texture2D(textureSource, gl_TexCoord[0].st + vec2(1.0*ScaleU.x, 1.0*ScaleU.y))*0.25;
			//color += texture2D(textureSource, gl_TexCoord[0].st + vec2(2.0*ScaleU.x, 2.0*ScaleU.y))*0.0625;

			//3*3
			//color += texture2D(textureSource, gl_TexCoord[0].st + vec2(-1.0*ScaleU.x, -1.0*ScaleU.y))*0.25;
			//color += texture2D(textureSource, gl_TexCoord[0].st + vec2(0.0, 0.0))*0.5;
			//color += texture2D(textureSource, gl_TexCoord[0].st + vec2(1.0*ScaleU.x, 1.0*ScaleU.y))*0.25;
			
			//1*1
			//color += texture2D(textureSource, gl_TexCoord[0].st + vec2(0.0, 0.0));

			
			//gl_FragColor = vec4(ScaleU.x*4096, ScaleU.y*4096 ,0 ,0);
			//gl_FragColor = vec4(1, 0, 0, 0);
			//gl_FragColor = texture2D(textureSource, gl_TexCoord[0].st);

			float t = 0.0;
			for (float i = -9.0; i < 10.0; i++)
			{
				float r = 1.0 + 2.0 * abs(i);
				float w = exp(-0.5*(r*r) / 100.0);
				t += w;
			}
			for (float i = -9.0; i < 10.0; i++)
			{
				float r = 1.0 + 2.0 * abs(i);
				float w = exp(-0.5*(r*r) / 100.0);
				color += texture2D(textureSource, gl_TexCoord[0].st + vec2(i*ScaleU.x, i*ScaleU.y))*w / t;
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