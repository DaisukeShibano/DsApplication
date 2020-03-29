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
		//varying vec4 vPos;
		varying vec3 worldPos;
		varying vec3 vNrm;
		varying vec4 vShadowCoord;	//!< シャドウデプスマップの参照用座標
		varying vec3 normalMapLight;
		varying vec3 normalMapLightEx[10];
		varying vec3 normalMapView;
		attribute vec3 tangent;
		uniform mat4 modelTransform;//!<描画モデル座標系
		uniform mat4 modelViewProjectionTransform;

		void main(void)
		{
			vNrm = normalize(gl_NormalMatrix*gl_Normal);	// 頂点法線
			vShadowCoord = gl_TextureMatrix[7] * modelTransform * gl_Vertex;	// 影用座標値(光源中心座標)

			// 描画用
			gl_Position = modelViewProjectionTransform * gl_Vertex;		// 頂点位置
			gl_FrontColor = gl_Color;				// 頂点色
			gl_TexCoord[0] = gl_MultiTexCoord0;		// 頂点テクスチャ座標

			//vPos = gl_Position;
			worldPos = vec3(modelTransform * gl_Vertex);

			//法線マップ用
			//http://marina.sys.wakayama-u.ac.jp/~tokoi/?date=20051014
			float tanL = vNrm[0] * vNrm[0] + vNrm[2] * vNrm[2] + 0.00001;
			float tanSq = sqrt(tanL);
			vec3 tangent;
			tangent[0] = vNrm[0] / tanSq;
			tangent[1] = 0.0;
			tangent[2] = -vNrm[2] / tanSq;


			vec4 p = gl_ModelViewMatrix * gl_Vertex;
			vec3 n = normalize(gl_NormalMatrix * gl_Normal);
			vec3 t = normalize(gl_NormalMatrix * tangent);
			vec3 b = cross(n, t);
			vec3 temp;
			temp.x = dot(p.xyz, t);
			temp.y = dot(p.xyz, b);
			temp.z = dot(p.xyz, n);
			normalMapView = -normalize(temp);//面座標系の頂点位置
			
			{//平行光源
				//vec3 l = normalize((gl_LightSource[0].position * p.w - gl_LightSource[0].position.w * p).xyz);
				vec3 l = normalize((gl_LightSource[0].position - p).xyz);
				temp.x = dot(l, t);
				temp.y = dot(l, b);
				temp.z = dot(l, n);
				normalMapLight = normalize(temp);//面座標系の光の方向
			}

			//追加光源分
			{
				vec3 pointPos = vec3(1.0, 0.0, -1.0);//モデルビュー行列が既にかかっている座標
				vec3 l = normalize(pointPos - p.xyz);
				temp.x = dot(l, t);
				temp.y = dot(l, b);
				temp.z = dot(l, n);
				normalMapLightEx[0] = normalize(temp);//面座標系の光の方向
			}
		}
	);


	/***************************************************
	@brief		フラグメントシェーダー
	***************************************************/
	static const char s_fragment[] = DS_SHADER_STR(
		// バーテックスシェーダから受け取る変数
		//varying vec4 vPos;
		varying vec3 worldPos;
		varying vec3 vNrm;
		varying vec4 vShadowCoord;
		varying vec3 normalMapLight;
		varying vec3 normalMapLightEx[10];
		varying vec3 normalMapView;

		uniform sampler2D texAlbedo;	//!< アルベドテクスチャ
		uniform sampler2D texNormal;	//!< ノーマルテクスチャ
		uniform sampler2D texSpecular;	//!< スペキュラテクスチャ
		uniform sampler2D depth_tex;	//!< デプス値テクスチャ
		uniform sampler2D depth_tex2;

		// 影の濃さ
		uniform float shadow_ambient;
		uniform float time;

		// テクスチャの有無
		uniform bool isUseColorTexture;// = true;//GLSL 1.10でエラー出るのでコメントアウト
		uniform bool isUseLight;// = true;
		uniform bool isUseNormalMap;// = true;
		uniform bool isUseWaveNormalMap;//=false;
		uniform bool isUseSpecularMap;// = true;

		float outSpecular;

		/*!
		* Phong反射モデルによるシェーディング
		* @return 表面反射色
		*/
		//vec4 PhongShading(const vec4 texColor)
		//{
		//	vec3 N = normalize(vNrm);			// 法線ベクトル
		//	vec3 L = normalize(gl_LightSource[0].position.xyz - vPos.xyz);	// ライトベクトル
		//
		//	// 環境光の計算
		//	//  - OpenGLが計算した光源強度と反射係数の積(gl_FrontLightProduct)を用いる．
		//	vec4 ambient = gl_FrontLightProduct[0].ambient;
		//
		//	// 拡散反射の計算
		//	float dcoef = max(dot(L, N), 0.0);
		//	//アニメっぽく
		//	//dcoef = (0.5 < dcoef) ? 1.0 : 0.9;
		//
		//	vec4 diffuse = gl_FrontLightProduct[0].diffuse*dcoef;
		//
		//
		//	// 鏡面反射の計算
		//	vec4 specular = vec4(0.0);
		//	if (dcoef > 0.0){
		//		vec3 V = normalize(-vPos.xyz);		// 視線ベクトル
		//
		//		// 反射ベクトルの計算(フォン反射モデル)
		//		vec3 R = reflect(-L, N);
		//		//vec3 R = 2.0*dot(N, L)*N-L;	// reflect関数を用いない場合
		//		float specularLight = pow(max(dot(R, V), 0.0), gl_FrontMaterial.shininess);
		//
		//		specular = gl_FrontLightProduct[0].specular*specularLight;
		//	}
		//	vec4 baseColor = (isUseColorTexture) ? (texColor) : (gl_Color);
		//	return (isUseLight) ? (baseColor*(ambient + diffuse) + specular) : (baseColor);
		//}

		/*
		法線を揺らがせる
		*/
		vec3 WaveNormalMap(const vec3 baseNormal)
		{
			float scale = 3.1415 * 5.0;
			float offset = time * scale;

			float noiseSpeedX = cos(time*1.5)*3.1415;//適当
			float noiseSpeedY = sin(time*0.5)*3.1415;//適当

			//gl_TexCoordは0～1.波長を調整
			float len = (800.0/20.0) * 3.1415;
			float noiseLenX = sin(gl_TexCoord[0].st.y*len*2.0)*1.0;//適当
			float noiseLenY = sin(gl_TexCoord[0].st.x*len*0.5)*2.0;//適当
			float x = sin(gl_TexCoord[0].st.x*(len + noiseLenX) + offset + noiseSpeedX)*0.5 + 0.5;//0～１へ調整
			float y = sin(gl_TexCoord[0].st.y*(len + noiseLenY) + offset + noiseSpeedY)*0.5 + 0.5;

			return normalize(vec3(x, y, x*y));
		}

		/*
		法線やスペキュラマップを適用
		なんやかんやバッファが追加されたのでポストプロセスでも可
		*/
		vec4 ApplyMap(const vec4 baseColor)
		{
			vec4 normalColor = isUseNormalMap ? texture2DProj(texNormal, gl_TexCoord[0]) : vec4(0.5, 0.5, 1.0, 1.0);
			vec3 fnormal = vec3(normalColor) * 2.0 - 1.0;//面座標系の法線

			if (isUseWaveNormalMap) {//法線を波で揺らす
				fnormal = WaveNormalMap(fnormal);
			}

			vec3 fview = normalize(normalMapView);
			vec4 specularColor = isUseSpecularMap ? texture2DProj(texSpecular, gl_TexCoord[0]) : gl_FrontLightProduct[0].specular;
			outSpecular = 0;

			//ポイントライト
			vec4 pointLight;
			{
				vec4 pointCol = vec4(1.0, 0.0, 0.0, 1.0);
				vec3 pointPos = vec3(1.0, 0.0, -1.0);//モデルビュー行列が既にかかっている座標
				float pointLen = length(pointPos - worldPos);
				float kc = 1.0;
				float k1 = 1.0;
				float k2 = 1.0;
				float attenuation = 1.0 / (kc + k1*pointLen + k2*pointLen*pointLen);
				
				vec3 flight = normalize(normalMapLightEx[0]);//面座標系の光の方向
				float diffuse = max(dot(flight, fnormal), 0.0);
				vec3 halfway = normalize(flight + fview);
				float specular = pow(max(dot(fnormal, halfway), 0.0), gl_FrontMaterial.shininess);
				
				pointLight = pointCol * (gl_FrontLightProduct[0].diffuse * diffuse + gl_FrontLightProduct[0].ambient + gl_FrontMaterial.emission)
					+ specularColor * specular;
				pointLight *= attenuation;
				outSpecular += attenuation * (specular * length(specularColor.rgb));
			}

			vec3 flight = normalize(normalMapLight);//面座標系の光の方向
			float diffuse = max(dot(flight, fnormal), 0.0);
			
			vec3 halfway = normalize(flight + fview);
			float specular = pow(max(dot(fnormal, halfway), 0.0), gl_FrontMaterial.shininess);

			vec4 ret;
			ret = pointLight;
			ret += baseColor * (gl_FrontLightProduct[0].diffuse * diffuse + gl_FrontLightProduct[0].ambient + gl_FrontMaterial.emission)
				+ specularColor * specular;

			outSpecular += specular * length(specularColor.rgb);
			return (isUseLight) ? (ret) : (baseColor);
		}

		/*
			影ぼやかし
		*/
		float ChebyshevUpperBound(vec4 shadow_coord, vec4 depth_tex_coord)
		{
			float distance = shadow_coord.z;//これが描画座標
			//distance = distance * 0.5 + 0.5;// This is done via a bias matrix in main.c
			//*0.5+0.5のバイアス行列かけてあるので0-1になってる

			//distanceが描画しようとするピクセルの光源視点からの深度
			//moments.xがシャドウマップの深度
			//distanceの方が小さい(=光源に距離が近い)場合は日向 


			// We retrive the two moments previously stored (depth and depth*depth)
			vec2 moments = depth_tex_coord.rg;

			// Surface is fully lit. as the current fragment is before the light occluder
			if (distance <= moments.x) {
				return 1.0;
			}

			if (vShadowCoord.w <= 0.0) {
				return 1.0;
			}

			// The fragment is either in shadow or penumbra. We now use chebyshev's upperBound to check
			// How likely this pixel is to be lit (p_max)
			float variance = moments.y - (moments.x*moments.x);//moments.yはmoment2 += 0.25*(dx*dx + dy*dy)だけmoments.xの２乗より多い
			variance = min(max(variance, 0.000002), 1.0);

			float d = distance - moments.x;//日向と日陰の距離が近いほど１（日向）に近づく
			float p_max = variance / (variance + d*d);//variance(エッジ)が大きければ大きいほどdの支配は少なくなる。

			return p_max;
		}

		/*
		* 影生成のための係数(影のあるところで1, それ以外で0)
		* @return 影係数(影のあるところで1, それ以外で0)
		*/
		float ShadowCoef(vec4 shadow_coord, vec4 depth_tex_coord)
		{
			// 光源からのデプス値(視点)
			float view_d = shadow_coord.z;//-0.0001;
			
			// 格納された光源からの最小デプス値を取得
			float light_d = depth_tex_coord.x;

			// 影で0,日向で1
			float shadow_coef = 1.0;
			if (vShadowCoord.w > 0.0){
				shadow_coef = light_d < view_d ? 0.0 : 1.0;
			}

			return shadow_coef;
		}

		void main(void)
		{
			vec4 texColor = (isUseColorTexture) ? (texture2D(texAlbedo, gl_TexCoord[0].st)) : (gl_Color); //影じゃない普通のテクスチャ
			//vec4 light_col = PhongShading(texColor);	// 表面反射色
			vec4 light_col = ApplyMap(texColor);

			// 光源座標における物体の位置
			vec4 shadow_coord = vShadowCoord / vShadowCoord.w;//これが描画しようとしている座標			

			//// 光源座標における物体の位置に相当する影テクスチャの値
			//vec4 depth_tex_coord = texture2D(depth_tex, shadow_coord.xy);
			//// 影影響係数
			//float shadow_coef = ShadowCoef(shadow_coord, depth_tex_coord);
			//gl_FragColor = (1.0-shadow_ambient)*shadow_coef*light_col
			//				+ (shadow_ambient) * light_col;//日陰の部分も色が出るように
			//gl_FragColor.w = texColor.w;//アルファ値は演算の対象外

			//影ぼやかし
			vec4 depth_tex_coord2 = texture2D(depth_tex2, shadow_coord.xy);//ぼかし要素が入ってる影テクスチャ
			float shadow_blurry = ChebyshevUpperBound(shadow_coord, depth_tex_coord2);
			float shadow_blurry_modify = min(shadow_ambient + shadow_blurry, 1.0);
			vec4 fragColor = vec4(shadow_blurry_modify)*light_col;

			//アルファ値は個別で算出
			//glColor4f()のアルファ値だけを適用する。フェードアウト用など
			fragColor.w = gl_Color.w * texColor.w;

			gl_FragData[0] = fragColor;
			gl_FragData[1] = fragColor;
			gl_FragData[2] = vec4(vec3(vNrm)*0.5 + 0.5, 1.0);
			gl_FragData[3] = vec4(outSpecular, 0.0, 0.0, 1.0);

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