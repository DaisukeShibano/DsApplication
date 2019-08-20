#include "DsPch.h"
#include "Graphics/Shader/ShaderSource/DsSSRShader.h"

using namespace DsLib;
#define DS_SHADER_STR(str) #str


namespace //SSR
{ 
	/***************************************************
	@brief		バーテックスシェーダー
	***************************************************/
	static const char s_vertex1[] = DS_SHADER_STR(
		void main(void)
		{
			gl_Position = ftransform();
			gl_TexCoord[0] = gl_MultiTexCoord0;
		}
	);
	/***************************************************
	@brief		フラグメントシェーダー
	***************************************************/
	static const char s_fragment1[] = DS_SHADER_STR(
		uniform sampler2D colTexEff;
		uniform sampler2D colTexOri;
		uniform sampler2D depTexOri;
		uniform sampler2D normalTex;
		uniform sampler2D specularDepthTex;
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

		/*
			vec3.x posが深度値よりも手前ならfalse、そうでないならtrue
			vec3.yz 判定ピクセル座標
		*/
		vec3 frontOrBack(vec4 pos)
		{
			vec3 ret = vec3(0);
			vec4 clip = projectionTransform * pos;//クリップ座標へ変換
			if (clip.w != 0.0) {
				vec2 hitPix = (clip.xy / clip.w)*0.5 + 0.5;//ピクセル位置へ変換

				if ((0.0 < hitPix.x) && (hitPix.x < 1.0) && (0.0 < hitPix.y) && (hitPix.y < 1.0)) {
					float hitPixDepth = texture2D(depTexOri, hitPix).z;//判定座標の深度値取得
					float rayDepth = clip.z / clip.w;

					//レイの方が奥にあればtrue
					ret.x = (hitPixDepth < rayDepth) ? 1.0 : 0.0;
					ret.y = hitPix.x;
					ret.z = hitPix.y;
				}
			}
			return ret;
		}


		void main(void)
		{
			vec3 pos = getWorldPosition(gl_TexCoord[0].st, texture2D(depTexOri, gl_TexCoord[0].st).z);
			vec3 normal = vec3(texture2D(normalTex, gl_TexCoord[0].st))*2.0 - 1.0;
			vec3 reflectDir = reflect(normalize(pos), normal);
			
			//レイを飛ばす
			vec3 start = pos;
			float rayLen = 50.0;//レイを飛ばす距離
			vec3 end = start + (reflectDir * rayLen);
			
			float iteration = 1000.0;//衝突判定回数
			float dLen = rayLen / iteration;
			
			float thickness = 0.01;//衝突判定のための擬似的な厚さ（単位は深度値と同じ）
			
			float distanceRate = 1.0;

			vec4 startClip4 = projectionTransform * vec4(start, 1.0);
			vec3 startClip = (startClip4.xyz / startClip4.w)*0.5 + 0.5;
			
			vec4 endClip4 = projectionTransform * vec4(end, 1.0);
			vec3 endClip = (endClip4.xyz / endClip4.w)*0.5 + 0.5;
			
			vec3 dVecClip = (endClip - startClip) / iteration;


			bool isHit = false;
			vec2 hitPix = vec2(0);
			
			//vec3 searchPos = end;
			//vec3 searchVec = end - start;
			//vec3 result;
			//for(int i=0; i < 10; ++i)
			//{
			//	result = frontOrBack( vec4(searchPos, 1.0) );
			//	searchVec = searchVec * 0.5;
			//	
			//	//２分岐探索
			//	if (0.5 < result.x) {//裏側
			//		searchPos = searchPos - searchVec;
			//	}
			//	else {//表側
			//		//初回で何にも裏側に貫通して無いならヒット無し確定
			//		if (i == 0) {
			//			isHit = false;
			//			break;
			//		}
			//		searchPos = searchPos + searchVec;
			//	}
			//}
			//hitPix.x = result.y;
			//hitPix.y = result.z;

			float iteEnd = rayLen - 0.001;//for文終了判定誤差のため
			float len = dLen;
			for (; len < iteEnd; len *= 1.1){
				
				//線形のデプス値があれば使える
				//startClip = startClip + dVecClip;
				//hitPix = startClip.xy;
				//
				//if ((0.0 < hitPix.x) && (hitPix.x < 1.0) && (0.0 < hitPix.y) && (hitPix.y < 1.0))
				//{
				//
				//	float hitDepth = texture2D(depTexOri, hitPix).z;//判定座標の深度値取得
				//	float rayDepth = startClip.z;
				//
				//	//レイの深度値と描画深度値を比較し、レイが重なっていればヒット
				//	if ((hitDepth < rayDepth) && (rayDepth < (hitDepth + thickness))) {
				//		isHit = true;
				//		break;
				//	}
				//}

				
				vec4 rayP = vec4(start + (reflectDir * len), 1.0);//判定座標のワールド座標
				vec4 rayClip = projectionTransform * rayP;//クリップ座標へ変換
				if (rayClip.w != 0.0) {
					hitPix = (rayClip.xy / rayClip.w)*0.5 + 0.5;//ピクセル位置へ変換
				
					if ((0.0 < hitPix.x) && (hitPix.x < 1.0) && (0.0 < hitPix.y) && (hitPix.y < 1.0))
					{
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

			//距離で減衰
			distanceRate = 1.0 - (len / rayLen);

			if(isHit){
				gl_FragColor = (texture2D(colTexOri, hitPix) * distanceRate) + (texture2D(colTexOri, gl_TexCoord[0].st)*(1.0-distanceRate));
				gl_FragColor.a = 1.0;
			}
			else {
				vec3 rgb = texture2D(colTexOri, gl_TexCoord[0].st).rgb;
				gl_FragColor = vec4(rgb, 0.0);
			}
		}
	);
}

namespace //SSR
{
	/***************************************************
	@brief		バーテックスシェーダー
	***************************************************/
	static const char s_vertex2[] = DS_SHADER_STR(
		void main(void)
		{
			gl_Position = ftransform();
			gl_TexCoord[0] = gl_MultiTexCoord0;
		}
	);

	/***************************************************
	@brief		フラグメントシェーダー
	***************************************************/
	static const char s_fragment2[] = DS_SHADER_STR(
		uniform sampler2D colTexEff;//ブラー適用
		uniform sampler2D colTexOri;//ssr直前
		uniform sampler2D specularDepthTex;

		void main(void)
		{
			vec4 col1 = texture2D(colTexEff, gl_TexCoord[0].st);
			vec4 col2 = texture2D(colTexOri, gl_TexCoord[0].st);
			float specular = texture2D(specularDepthTex, gl_TexCoord[0].st).x;

			if (0.5 < col1.a) {
				gl_FragColor = col1 * specular + col2 * (1.0 - specular);
			}
			else {
				gl_FragColor = vec4(col2.rgb, 1.0);
			}
		}
	);

}

const char* DsLib::GetSSRVertexShader1()
{
	return s_vertex1;
}

const char* DsLib::GetSSRFragmentShader1()
{
	return s_fragment1;
}

const char* DsLib::GetSSRVertexShader2()
{
	return s_vertex2;
}

const char* DsLib::GetSSRFragmentShader2()
{
	return s_fragment2;
}
