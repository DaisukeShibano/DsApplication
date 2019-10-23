#include "DsPch.h"
#include "Graphics/Shader/ComputeShaderSource/DsRayTraceShader.h"

using namespace DsLib;
#define DS_SHADER_STR(str) #str


namespace
{
	static const char s_src[] = DS_SHADER_STR(
		//image2D destTex;
		float a;
		layout(local_size_x = 16, local_size_y = 16) in;
		void main(void)
		{
			a = 1.0;
		//	imageStore(destTex, ivec2(0, 0), vec4(0.0, 0.0, 0.0, 0.0));
		}
	);
}

const char* DsLib::GetRayTraceShader()
{
	return s_src;
}
