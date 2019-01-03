#include "DsPch.h"
#include "Graphics/Shader/ShaderSource/DsDepthFieldShader.h"

using namespace DsLib;
#define DS_SHADER_STR(str) #str


namespace//�f�v�X�l�Z�o
{
	/***************************************************
	@brief		�o�[�e�b�N�X�V�F�[�_�[
	***************************************************/
	static const char s_vertex1[] = DS_SHADER_STR(
		void main(void)
		{
			gl_Position = ftransform();
			gl_TexCoord[0] = gl_MultiTexCoord0;
		}
	);
	/***************************************************
	@brief		�t���O�����g�V�F�[�_�[
	***************************************************/
	static const char s_fragment1[] = DS_SHADER_STR(
		uniform sampler2D depTexOri;

		void main(void)
		{
			//�����ŋ��߂��[�x�l���ڂ₯���摜�Ƃ̃u�����h���ɂȂ�
			//�s���g�t�߂�0�ɂ��Ă������牓���قǍ�������

			gl_FragColor = texture2D(depTexOri, gl_TexCoord[0].st);
		}
	);
}

//�u���[�͕ʂ̃V�F�[�_�[�g��

namespace//����
{
	/***************************************************
	@brief		�o�[�e�b�N�X�V�F�[�_�[
	***************************************************/
	static const char s_vertex2[] = DS_SHADER_STR(
		void main(void)
		{
			gl_Position = ftransform();
			gl_TexCoord[0] = gl_MultiTexCoord0;
		}
	);
	/***************************************************
	@brief		�t���O�����g�V�F�[�_�[
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

