#include "DsPch.h"
#include "Graphics/Shader/ShaderSource/DsSSAOShader.h"

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
		uniform sampler2D colTexEff;
		uniform sampler2D depTexOri;

		void main(void)
		{
			float edge = 0.04;//�[�x�l���߂����̂����e����
			float fade = 0.02;//�e����Ȃ��Ƃ���ւ̃t�F�[�h����

			const int sampleNum = 15;
			const float sampleNumf = float(sampleNum);
			float offset[sampleNum];
			offset[0] = 1.0 / 800.0;//��̃s�N�Z���P��
			offset[1] = 1.0 / 800.0;
			offset[2] = 1.0 / 800.0;
			offset[3] = 5.0 / 800.0;
			offset[4] = 5.0 / 800.0;
			offset[5] = 5.0 / 800.0;
			offset[6] = 9.0 / 800.0;
			offset[7] = 9.0 / 800.0;
			offset[8] = 9.0 / 800.0;
			offset[9] = 13.0 / 800.0;
			offset[10]= 13.0 / 800.0;
			offset[11]= 13.0 / 800.0;
			offset[12] = 17.0 / 800.0;
			offset[13] = 17.0 / 800.0;
			offset[14] = 17.0 / 800.0;




			vec2 sampOffset[sampleNum];
			//sin(45)=0.707 cos(45)=0.707
			sampOffset[0] = vec2(offset[0]*0.707, offset[0]*0.707);
			sampOffset[1] = vec2(offset[1], 0.0);
			sampOffset[2] = vec2(offset[2] * 0.707, -offset[2] * 0.707);
			sampOffset[3] = vec2(offset[3] * 0.707, offset[3] * 0.707);
			sampOffset[4] = vec2(offset[4], 0.0);
			sampOffset[5] = vec2(offset[5] * 0.707, -offset[5] * 0.707);
			sampOffset[6] = vec2(offset[6] * 0.707, offset[6] * 0.707);
			sampOffset[7] = vec2(offset[7], 0.0);
			sampOffset[8] = vec2(offset[8] * 0.707, -offset[8] * 0.707);
			sampOffset[9] = vec2(offset[9] * 0.707, offset[6] * 0.707);
			sampOffset[10]= vec2(offset[10], 0.0);
			sampOffset[11]= vec2(offset[11] * 0.707, -offset[8] * 0.707);
			sampOffset[12] = vec2(offset[12] * 0.707, offset[6] * 0.707);
			sampOffset[13] = vec2(offset[13], 0.0);
			sampOffset[14] = vec2(offset[14] * 0.707, -offset[8] * 0.707);

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
				//�Ȃ����������ڂ��悩�����̂ŃR�����g�A�E�g
				//distBias += dist1;
				//distBias += dist2;
				
				float s = (p != baseDepth) ? atan(offset[idx], baseDepth - p) : 3.1415;
				float sr = (pr != baseDepth) ? atan(offset[idx], baseDepth - pr) : 3.1415;
				float sSum = min(3.1415, s + sr);

				//���l�ȏ�[�x������Ă�Ȃ�Օ��Ƃ݂Ȃ��Ȃ�
				//if ( (edge < dist1) || (edge < dist2) ){
				//	sSum = 3.1415;
				//}
				//���E���͂����肵������

				//���l�ȏ�[�x������Ă���ꍇ�͏��X�ɖ��邭
				if (edge < dist1) {
					sSum += (dist1 - edge)/fade;
				}
				if (edge < dist2) {
					sSum += (dist2 - edge) / fade;
				}
				sSum = min(sSum, 3.1415);


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
			sResult = sResult /3.1415;//�p�x������=�Օ����ꗦ�������ˏ������قǉe

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
