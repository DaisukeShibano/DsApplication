#include "DsPch.h"
#include "Graphics/Shader/ShaderSource/DsSSRShader.h"

using namespace DsLib;
#define DS_SHADER_STR(str) #str


namespace
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
		uniform sampler2D colTexOri;
		uniform sampler2D depTexOri;
		uniform sampler2D normalTex;
		uniform mat4 projectionTransform;
		uniform mat4 projectionInverseTransform;
		//uniform mat4 modelViewProjectionTransform;
		//uniform mat4 modelViewProjectionInverseTransform;
		//uniform vec3 cameraPosition;

		vec3 getWorldPosition(vec2 texCoord, float depth)
		{
			vec2 screen = texCoord * 2.0 - 1.0;
			vec4 sPos = vec4(screen.x, screen.y, depth, 1.0);
			vec4 wPos = projectionInverseTransform * sPos;
			vec3 ret = wPos.xyz / wPos.w;
			return ret;
		}


		void main(void)
		{
			vec3 pos = getWorldPosition(gl_TexCoord[0].st, texture2D(depTexOri, gl_TexCoord[0].st).z);
			vec3 normal = vec3(texture2D(normalTex, gl_TexCoord[0].st))*2.0 - 1.0;
			vec3 reflectDir = reflect(normalize(pos), normal);

			//レイを飛ばす
			vec3 start = pos;
			float rayLen = 3.0;//レイを飛ばす距離
			float iteration = 40.0;//衝突判定回数
			float dLen = rayLen / iteration;
			
			float thickness = 0.01;//衝突判定のための擬似的な厚さ（単位は深度値と同じ）
			
			bool isHit = false;
			vec2 hitPix;
			float iteEnd = rayLen - 0.001;//for文終了判定誤差のため
			for (float len = dLen; len < iteEnd; len += dLen){
				vec4 rayP = vec4(start + (reflectDir * len), 1.0);//判定座標のワールド座標
				vec4 rayClip = projectionTransform * rayP;//クリップ座標へ変換
				if (rayClip.w != 0.0) {
					hitPix = (rayClip.xy / rayClip.w)*0.5 + 0.5;//ピクセル位置へ変換

					if ((0.0 < hitPix.x) && (hitPix.x < 1.0) && (0.0 < hitPix.y) && (hitPix.y < 1.0)) {

						float hitDepth = texture2D(depTexOri, hitPix).z;//判定座標の深度値取得
						float rayDepth = rayClip.z / rayClip.w;
						
						//レイの深度値と描画深度値を比較し、レイが重なっていればヒット
						if ((hitDepth < rayDepth) && (rayDepth < (hitDepth + thickness))) {
							isHit = true;
							break;
						}
					}
				}
			}

			if(isHit){
				gl_FragColor = texture2D(colTexOri, hitPix);
			}
			else {
				gl_FragColor = texture2D(colTexOri, gl_TexCoord[0].st);
			}
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
