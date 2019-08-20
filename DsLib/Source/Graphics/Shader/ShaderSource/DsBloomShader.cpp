#include "DsPch.h"
#include "Graphics/Shader/ShaderSource/DsBloomShader.h"

using namespace DsLib;
#define DS_SHADER_STR(str) #str


namespace//���P�x���o
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
		uniform sampler2D colTexEff;
		uniform sampler2D specularDepthTex;

		void main(void)
		{
			float thre = (1.0 - texture2D(specularDepthTex, gl_TexCoord[0].st).x) * 0.8;
			vec3 color = texture2D(colTexEff, gl_TexCoord[0].st).rgb;
			vec3 lumiVec = vec3(0.298912, 0.586611, 0.114478);
			float luminance = dot(lumiVec, color);

			//�P�x��臒l�l�ɑ�������rgb�̒l�����߂�
			float subColor = thre / (lumiVec.r + lumiVec.g + lumiVec.b);

			vec3 texel = (thre < luminance) ? (color - subColor) : vec3(0.0);
			//vec3 texel = max( vec3(0.0), color - thre);//�������ł�����܂�ς��Ȃ�
			gl_FragColor = vec4(texel, 1.0);
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
		uniform sampler2D colTexOri;

		void main(void)
		{
			vec4 texel = texture2D(colTexEff, gl_TexCoord[0].st);
			vec4 texelOri = texture2D(colTexOri, gl_TexCoord[0].st);
			gl_FragColor = texel + texelOri;
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

