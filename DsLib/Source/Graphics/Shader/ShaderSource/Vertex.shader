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