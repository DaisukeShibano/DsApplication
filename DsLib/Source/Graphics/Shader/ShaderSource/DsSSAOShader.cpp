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

			const int sampleNum = 6;
			const float sampleNumf = float(sampleNum);
			float offset[sampleNum];
			offset[0] = 5.0 / 800.0;//��̃s�N�Z���P��
			offset[1] = 5.0 / 800.0;
			offset[2] = 5.0 / 800.0;
			offset[3] = 1.0 / 800.0;
			offset[4] = 1.0 / 800.0;
			offset[5] = 1.0 / 800.0;


			vec2 sampOffset[sampleNum];
			//sin(45)=0.707 cos(45)=0.707
			sampOffset[0] = vec2(offset[0]*0.707, offset[0]*0.707);
			sampOffset[1] = vec2(offset[1], 0.0);
			sampOffset[2] = vec2(offset[2] * 0.707, -offset[2] * 0.707);
			sampOffset[3] = vec2(offset[3] * 0.707, offset[3] * 0.707);
			sampOffset[4] = vec2(offset[4], 0.0);
			sampOffset[5] = vec2(offset[5] * 0.707, -offset[5] * 0.707);


			float baseDepth = texture2D(depTexOri, gl_TexCoord[0].st).x;
			float distBias = 0.0;
			float sResult = 0.0;

			for (int idx = 0; idx < sampleNum; ++idx) {
				float p = texture2D(depTexOri, gl_TexCoord[0].st + sampOffset[idx]).x;
				float pr = texture2D(depTexOri, gl_TexCoord[0].st - sampOffset[idx]).x;
				
				//�[�x�l������Ă�قǖ��邭����
				//edge�ȏ�͉e���S���t���Ȃ�
				float dist1 = abs(p - baseDepth);
				float dist2 = abs(pr - baseDepth);
				distBias += dist1;
				distBias += dist2;
				
				float s = (p != baseDepth) ? atan(offset[idx], baseDepth - p) : 3.1415;
				float sr = (pr != baseDepth) ? atan(offset[idx], baseDepth - pr) : 3.1415;
				float sSum = min(3.1415, s + sr);

				//���l�ȏ�[�x������Ă�Ȃ�Օ��Ƃ݂Ȃ��Ȃ�
				if ((edge < dist1) || (edge < dist2) ){
					sSum = 3.1415;
				}
				sResult += sSum;
			}

			//���ς��Ƃ���0.0�`1.0��
			distBias /= sampleNumf * 2.0;
			distBias = min(edge, distBias) / edge;
			//http://d.hatena.ne.jp/nakamura001/20111117/1321539246
			//�yCubic�z
			float t = distBias;
			float b = 0.0;
			float c = 1.0;
			float d = 1.0;
			t /= d;
			distBias = c * t*t*t + b;


			sResult /= sampleNumf;
			sResult = sResult /3.1415;//�p�x������=�Օ����ꗦ������

			sResult = sResult * sResult;//����
			sResult = min(1.0, sResult + distBias);//�����Ŗ��邭�Ȃ镪���v���X
			
			vec4 srcCol = texture2D(colTexEff, gl_TexCoord[0].st);
			gl_FragColor = vec4(srcCol.rgb*sResult, srcCol.w);
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
