#include "DsPch.h"
#include "Graphics/Shader/ShaderSource/DsSSAOShader.h"

using namespace DsLib;
#define DS_SHADER_STR(str) #str


namespace//高輝度抽出
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
		uniform sampler2D colTexEff;
		uniform sampler2D depTexOri;

		void main(void)
		{
			float edge = 0.02;//深度値が近いものだけ影つける

			const int sampleNum = 6;
			const float sampleNumf = float(sampleNum);
			float offset[sampleNum];
			offset[0] = 5.0 / 800.0;//大体ピクセル単位
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
				
				//深度値が離れてるほど明るくする
				//edge以上は影が全く付かない
				float dist1 = abs(p - baseDepth);
				float dist2 = abs(pr - baseDepth);
				distBias += dist1;
				distBias += dist2;
				
				float s = (p != baseDepth) ? atan(offset[idx], baseDepth - p) : 3.1415;
				float sr = (pr != baseDepth) ? atan(offset[idx], baseDepth - pr) : 3.1415;
				float sSum = min(3.1415, s + sr);

				//一定値以上深度が離れてるなら遮蔽とみなさない
				if ((edge < dist1) || (edge < dist2) ){
					sSum = 3.1415;
				}
				sResult += sSum;
			}

			//平均をとって0.0～1.0へ
			distBias /= sampleNumf * 2.0;
			distBias = min(edge, distBias) / edge;
			//http://d.hatena.ne.jp/nakamura001/20111117/1321539246
			//【Cubic】
			float t = distBias;
			float b = 0.0;
			float c = 1.0;
			float d = 1.0;
			t /= d;
			distBias = c * t*t*t + b;


			sResult /= sampleNumf;
			sResult = sResult /3.1415;//角度が狭い=遮蔽され率が高い

			sResult = sResult * sResult;//強調
			sResult = min(1.0, sResult + distBias);//距離で明るくなる分をプラス
			
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
