#include "DsPch.h"
#include "Graphics/Shader/ShaderSource/DsSSAOShader.h"

using namespace DsLib;
#define DS_SHADER_STR(str) #str


namespace//���P�x���o
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
		uniform sampler2D colTexEff;
		uniform sampler2D depTexOri;

		void main(void)
		{
			float edge = 0.02;//�[�x�l���߂����̂����e����
			float offset = 4.0/800.0;//��̃s�N�Z���P��

			vec2 sampOffset1 = vec2(offset, 0.00);
			vec2 sampOffset2 = vec2(0.00, offset);

			float baseDepth = texture2D(depTexOri, gl_TexCoord[0].st).x;

			float p1 = texture2D(depTexOri, gl_TexCoord[0].st + sampOffset1).x;
			float p1r = texture2D(depTexOri, gl_TexCoord[0].st - sampOffset1).x;
			float p2 = texture2D(depTexOri, gl_TexCoord[0].st + sampOffset2).x;
			float p2r = texture2D(depTexOri, gl_TexCoord[0].st - sampOffset2).x;


			//�[�x�l������Ă�قǖ��邭����
			//edge�ȏ�͉e���S���t���Ȃ�
			float distBias = 0.0;
			distBias += abs(p1 - baseDepth);
			distBias += abs(p1r - baseDepth);
			distBias += abs(p2 - baseDepth);
			distBias += abs(p2r - baseDepth);
			distBias /= 4.0;
			distBias = min(edge, distBias) / edge;
			float t = distBias;
			float b = 0.0;
			float c = 1.0;
			float d = 1.0;

			//http://d.hatena.ne.jp/nakamura001/20111117/1321539246
			//�yCubic�z
			t /= d;
			distBias = c * t*t*t + b;


			float s1 = (p1 != baseDepth) ? atan(offset, baseDepth - p1) : 3.1415;
			float s1r = (p1r != baseDepth) ? atan(offset, baseDepth - p1r) : 3.1415;
			float s1Sum = min(3.1415, s1 + s1r);

			float s2 = (p2 != baseDepth) ? atan(offset, baseDepth - p2) : 3.1415;
			float s2r = (p2r != baseDepth) ? atan(offset, baseDepth - p2r) : 3.1415;
			float s2Sum = min(3.1415, s2 + s2r);

			float s = (s1Sum + s2Sum) / 2.0;
			s = s/3.1415;//�p�x������=�Օ����ꗦ������

			s = min(1.0, s + distBias);//�����Ŗ��邭�Ȃ镪���v���X
			
			vec4 srcCol = texture2D(colTexEff, gl_TexCoord[0].st);
			gl_FragColor = vec4(srcCol.rgb*s, srcCol.w);
		}
	);
}

const char* DsLib::GetSSAOVertexShader()
{
	return s_vertex;
}

const char* DsLib::GetSSAOFragmentShader()
{
	return s_fragment;
}
