#include "DsPch.h"
#include "Graphics/Shader/ShaderSource/DsSSRShader.h"

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

		void main(void)
		{
			
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
