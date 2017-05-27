#include "DsPch.h"
#ifndef _DS_DEFAULT_SHADER_H_
#include "Graphics/Shader/ShaderSource/DsDefaultShader.h"
#endif

using namespace DsLib;
#define DS_SHADER_STR(str) #str

namespace
{
	/***************************************************
	@brief		バーテックスシェーダー
	***************************************************/
	static const char s_vertex[] = DS_SHADER_STR(
		// フラグメントシェーダに値を渡すための変数
		varying vec4 vPos;
		varying vec3 vNrm;
		varying vec4 vShadowCoord;	//!< シャドウデプスマップの参照用座標

		void main(void)
		{
			// フラグメントシェーダでの計算用(モデルビュー変換のみ)
			vPos = gl_ModelViewMatrix*gl_Vertex;			// 頂点位置
			vNrm = normalize(gl_NormalMatrix*gl_Normal);	// 頂点法線
			vShadowCoord = gl_TextureMatrix[7] * gl_ModelViewMatrix*gl_Vertex;	// 影用座標値(光源中心座標)

			// 描画用
			gl_Position = gl_ProjectionMatrix*vPos;	// 頂点位置
			gl_FrontColor = gl_Color;				// 頂点色
			gl_TexCoord[0] = gl_MultiTexCoord0;		// 頂点テクスチャ座標
		}
	);


	/***************************************************
	@brief		フラグメントシェーダー
	***************************************************/
	static const char s_fragment[] = DS_SHADER_STR(
		// バーテックスシェーダから受け取る変数
		varying vec4 vPos;
		varying vec3 vNrm;
		varying vec4 vShadowCoord;


		// GLから設定される定数(uniform)
		uniform sampler2D tex;			//!< 通常テクスチャ
		uniform sampler2D depth_tex;	//!< デプス値テクスチャ

		// 影の濃さ
		uniform float shadow_ambient;

		// テクスチャの有無
		uniform bool isUseColorTexture=true;
		uniform bool isUseLight = true;

		/*!
		* Phong反射モデルによるシェーディング
		* @return 表面反射色
		*/
		vec4 PhongShading(const vec4 texColor)
		{
			vec3 N = normalize(vNrm);			// 法線ベクトル
			vec3 L = normalize(gl_LightSource[0].position.xyz - vPos.xyz);	// ライトベクトル

			// 環境光の計算
			//  - OpenGLが計算した光源強度と反射係数の積(gl_FrontLightProduct)を用いる．
			vec4 ambient = gl_FrontLightProduct[0].ambient;

			// 拡散反射の計算
			float dcoef = max(dot(L, N), 0.0);
			vec4 diffuse = gl_FrontLightProduct[0].diffuse*dcoef;


			// 鏡面反射の計算
			vec4 specular = vec4(0.0);
			if (dcoef > 0.0){
				vec3 V = normalize(-vPos.xyz);		// 視線ベクトル

				// 反射ベクトルの計算(フォン反射モデル)
				vec3 R = reflect(-L, N);
				//vec3 R = 2.0*dot(N, L)*N-L;	// reflect関数を用いない場合
				float specularLight = pow(max(dot(R, V), 0.0), gl_FrontMaterial.shininess);

				specular = gl_FrontLightProduct[0].specular*specularLight;
			}
			vec4 baseColor = (isUseColorTexture) ? (texColor) : (gl_Color);
			return (isUseLight) ? (baseColor*(ambient + diffuse) + specular) : (baseColor);
		}

		/*
			影ぼやかし
		*/
		float ChebyshevUpperBound(float distance, vec4 shadowCoordPostW)
		{
			// We retrive the two moments previously stored (depth and depth*depth)
			vec2 moments = texture2D(depth_tex, shadowCoordPostW.xy).rg;

			// Surface is fully lit. as the current fragment is before the light occluder
			if (distance <= moments.x) {
				return 1.0;
			}

			// The fragment is either in shadow or penumbra. We now use chebyshev's upperBound to check
			// How likely this pixel is to be lit (p_max)
			float variance = moments.y - (moments.x*moments.x);
			variance = max(variance, 0.00002);

			float d = distance - moments.x;
			float p_max = variance / (variance + d*d);

			return p_max;
		}

		/*!
		* 影生成のための係数(影のあるところで1, それ以外で0)
		* @return 影係数(影のあるところで1, それ以外で0)
		*/
		float ShadowCoef(void)
		{
			// 光源座標
			vec4 shadow_coord1 = vShadowCoord / vShadowCoord.w;

			// 光源からのデプス値(視点)
			float view_d = shadow_coord1.z;//-0.0001;

			// 格納された光源からの最小デプス値を取得
			float light_d = texture2D(depth_tex, shadow_coord1.xy).x;

			// 影で0,日向で1
			float shadow_coef = 1.0;
			if (vShadowCoord.w > 0.0){
				shadow_coef = light_d < view_d ? 0.0 : 1.0;
			}

			//影ぼやかし
			//float shadow_blurry = ChebyshevUpperBound(shadow_coord1.z, shadow_coord1);

			return shadow_coef;
		}

		void main(void)
		{
			vec4 texColor = texture2D(tex, gl_TexCoord[0].st); //影じゃない普通のテクスチャ
			vec4 light_col = PhongShading(texColor);	// 表面反射色
			float shadow_coef = ShadowCoef();	// 影影響係数

			// 出力
			gl_FragColor = shadow_ambient*shadow_coef*light_col
							+ (1.0 - shadow_ambient) *  light_col;//日陰の部分も色が出るように
			gl_FragColor.w = texColor.w;//アルファ値は演算の対象外
		}
	);
}

const char* DsLib::GetDefaultVertexShader()
{
	return s_vertex;
}

const char* DsLib::GetDefaultFragmentShader()
{
	return s_fragment;
}