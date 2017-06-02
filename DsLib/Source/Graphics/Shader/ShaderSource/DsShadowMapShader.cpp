#include "DsPch.h"
#ifndef _DS_SHADOW_MAP_SHADER_H_
#include "Graphics/Shader/ShaderSource/DsShadowMapShader.h"
#endif

using namespace DsLib;
#define DS_SHADER_STR(str) #str

namespace
{
	/***************************************************
	@brief		バーテックスシェーダー
	***************************************************/
	static const char s_vertex[] = DS_SHADER_STR(
		//影ぼやかしと試し中
		varying vec4 v_position;

		void main(void)
		{
			gl_Position = ftransform();
			v_position = gl_Position;
		}
	);


	/***************************************************
	@brief		フラグメントシェーダー
	***************************************************/
	static const char s_fragment[] = DS_SHADER_STR(
		varying vec4 v_position;

		void main(void)
		{
			float depth = v_position.z / v_position.w;
			depth = depth * 0.5 + 0.5;			//Don't forget to move away from unit cube ([-1,1]) to [0,1] coordinate system
			
			float moment1 = depth;
			float moment2 = depth * depth;
			
			// Adjusting moments (this is sort of bias per pixel) using partial derivative
			float dx = dFdx(depth);
			float dy = dFdy(depth);
			moment2 += 0.25*(dx*dx + dy*dy);
			
			gl_FragColor = vec4(moment1, moment2, 0.0, 0.0);
		}
	);
}

const char* DsLib::GetShadowMapVertexShader()
{
	return s_vertex;
}

const char* DsLib::GetShadowMapFragmentShader()
{
	return s_fragment;
}