#include "DsPch.h"
#ifndef _DS_DEFAULT_SHADER_H_
#include "Graphics/Shader/ShaderSource/DsDefaultShader.h"
#endif

using namespace DsLib;
#define DS_SHADER_STR(str) #str

namespace
{
	/***************************************************
	@brief		�o�[�e�b�N�X�V�F�[�_�[
	***************************************************/
	static const char s_vertex[] = DS_SHADER_STR(
		// �t���O�����g�V�F�[�_�ɒl��n�����߂̕ϐ�
		//varying vec4 vPos;
		varying vec3 worldPos;
		varying vec3 vNrm;
		varying vec4 vShadowCoord;	//!< �V���h�E�f�v�X�}�b�v�̎Q�Ɨp���W
		varying vec3 normalMapLight;
		varying vec3 normalMapLightEx[10];
		varying vec3 normalMapView;
		attribute vec3 tangent;
		uniform mat4 modelTransform;//!<�`�惂�f�����W�n
		uniform mat4 modelViewProjectionTransform;

		void main(void)
		{
			vNrm = normalize(gl_NormalMatrix*gl_Normal);	// ���_�@��
			vShadowCoord = gl_TextureMatrix[7] * modelTransform * gl_Vertex;	// �e�p���W�l(�������S���W)

			// �`��p
			gl_Position = modelViewProjectionTransform * gl_Vertex;		// ���_�ʒu
			gl_FrontColor = gl_Color;				// ���_�F
			gl_TexCoord[0] = gl_MultiTexCoord0;		// ���_�e�N�X�`�����W

			//vPos = gl_Position;
			worldPos = vec3(modelTransform * gl_Vertex);

			//�@���}�b�v�p
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
			normalMapView = -normalize(temp);//�ʍ��W�n�̒��_�ʒu
			
			{//���s����
				//vec3 l = normalize((gl_LightSource[0].position * p.w - gl_LightSource[0].position.w * p).xyz);
				vec3 l = normalize((gl_LightSource[0].position - p).xyz);
				temp.x = dot(l, t);
				temp.y = dot(l, b);
				temp.z = dot(l, n);
				normalMapLight = normalize(temp);//�ʍ��W�n�̌��̕���
			}

			//�ǉ�������
			{
				vec3 pointPos = vec3(1.0, 0.0, -1.0);//���f���r���[�s�񂪊��ɂ������Ă�����W
				vec3 l = normalize(pointPos - p.xyz);
				temp.x = dot(l, t);
				temp.y = dot(l, b);
				temp.z = dot(l, n);
				normalMapLightEx[0] = normalize(temp);//�ʍ��W�n�̌��̕���
			}
		}
	);


	/***************************************************
	@brief		�t���O�����g�V�F�[�_�[
	***************************************************/
	static const char s_fragment[] = DS_SHADER_STR(
		// �o�[�e�b�N�X�V�F�[�_����󂯎��ϐ�
		//varying vec4 vPos;
		varying vec3 worldPos;
		varying vec3 vNrm;
		varying vec4 vShadowCoord;
		varying vec3 normalMapLight;
		varying vec3 normalMapLightEx[10];
		varying vec3 normalMapView;

		uniform sampler2D texAlbedo;	//!< �A���x�h�e�N�X�`��
		uniform sampler2D texNormal;	//!< �m�[�}���e�N�X�`��
		uniform sampler2D texSpecular;	//!< �X�y�L�����e�N�X�`��
		uniform sampler2D depth_tex;	//!< �f�v�X�l�e�N�X�`��
		uniform sampler2D depth_tex2;

		// �e�̔Z��
		uniform float shadow_ambient;
		uniform float time;

		// �e�N�X�`���̗L��
		uniform bool isUseColorTexture;// = true;//GLSL 1.10�ŃG���[�o��̂ŃR�����g�A�E�g
		uniform bool isUseLight;// = true;
		uniform bool isUseNormalMap;// = true;
		uniform bool isUseWaveNormalMap;//=false;
		uniform bool isUseSpecularMap;// = true;

		float outSpecular;

		/*!
		* Phong���˃��f���ɂ��V�F�[�f�B���O
		* @return �\�ʔ��ːF
		*/
		//vec4 PhongShading(const vec4 texColor)
		//{
		//	vec3 N = normalize(vNrm);			// �@���x�N�g��
		//	vec3 L = normalize(gl_LightSource[0].position.xyz - vPos.xyz);	// ���C�g�x�N�g��
		//
		//	// �����̌v�Z
		//	//  - OpenGL���v�Z�����������x�Ɣ��ˌW���̐�(gl_FrontLightProduct)��p����D
		//	vec4 ambient = gl_FrontLightProduct[0].ambient;
		//
		//	// �g�U���˂̌v�Z
		//	float dcoef = max(dot(L, N), 0.0);
		//	//�A�j�����ۂ�
		//	//dcoef = (0.5 < dcoef) ? 1.0 : 0.9;
		//
		//	vec4 diffuse = gl_FrontLightProduct[0].diffuse*dcoef;
		//
		//
		//	// ���ʔ��˂̌v�Z
		//	vec4 specular = vec4(0.0);
		//	if (dcoef > 0.0){
		//		vec3 V = normalize(-vPos.xyz);		// �����x�N�g��
		//
		//		// ���˃x�N�g���̌v�Z(�t�H�����˃��f��)
		//		vec3 R = reflect(-L, N);
		//		//vec3 R = 2.0*dot(N, L)*N-L;	// reflect�֐���p���Ȃ��ꍇ
		//		float specularLight = pow(max(dot(R, V), 0.0), gl_FrontMaterial.shininess);
		//
		//		specular = gl_FrontLightProduct[0].specular*specularLight;
		//	}
		//	vec4 baseColor = (isUseColorTexture) ? (texColor) : (gl_Color);
		//	return (isUseLight) ? (baseColor*(ambient + diffuse) + specular) : (baseColor);
		//}

		/*
		�@����h�炪����
		*/
		vec3 WaveNormalMap(const vec3 baseNormal)
		{
			float scale = 3.1415 * 5.0;
			float offset = time * scale;

			float noiseSpeedX = cos(time*1.5)*3.1415;//�K��
			float noiseSpeedY = sin(time*0.5)*3.1415;//�K��

			//gl_TexCoord��0�`1.�g���𒲐�
			float len = (800.0/20.0) * 3.1415;
			float noiseLenX = sin(gl_TexCoord[0].st.y*len*2.0)*1.0;//�K��
			float noiseLenY = sin(gl_TexCoord[0].st.x*len*0.5)*2.0;//�K��
			float x = sin(gl_TexCoord[0].st.x*(len + noiseLenX) + offset + noiseSpeedX)*0.5 + 0.5;//0�`�P�֒���
			float y = sin(gl_TexCoord[0].st.y*(len + noiseLenY) + offset + noiseSpeedY)*0.5 + 0.5;

			return normalize(vec3(x, y, x*y));
		}

		/*
		�@����X�y�L�����}�b�v��K�p
		�Ȃ�₩���o�b�t�@���ǉ����ꂽ�̂Ń|�X�g�v���Z�X�ł���
		*/
		vec4 ApplyMap(const vec4 baseColor)
		{
			vec4 normalColor = isUseNormalMap ? texture2DProj(texNormal, gl_TexCoord[0]) : vec4(0.5, 0.5, 1.0, 1.0);
			vec3 fnormal = vec3(normalColor) * 2.0 - 1.0;//�ʍ��W�n�̖@��

			if (isUseWaveNormalMap) {//�@����g�ŗh�炷
				fnormal = WaveNormalMap(fnormal);
			}

			vec3 fview = normalize(normalMapView);
			vec4 specularColor = isUseSpecularMap ? texture2DProj(texSpecular, gl_TexCoord[0]) : gl_FrontLightProduct[0].specular;
			outSpecular = 0;

			//�|�C���g���C�g
			vec4 pointLight;
			{
				vec4 pointCol = vec4(1.0, 0.0, 0.0, 1.0);
				vec3 pointPos = vec3(1.0, 0.0, -1.0);//���f���r���[�s�񂪊��ɂ������Ă�����W
				float pointLen = length(pointPos - worldPos);
				float kc = 1.0;
				float k1 = 1.0;
				float k2 = 1.0;
				float attenuation = 1.0 / (kc + k1*pointLen + k2*pointLen*pointLen);
				
				vec3 flight = normalize(normalMapLightEx[0]);//�ʍ��W�n�̌��̕���
				float diffuse = max(dot(flight, fnormal), 0.0);
				vec3 halfway = normalize(flight + fview);
				float specular = pow(max(dot(fnormal, halfway), 0.0), gl_FrontMaterial.shininess);
				
				pointLight = pointCol * (gl_FrontLightProduct[0].diffuse * diffuse + gl_FrontLightProduct[0].ambient + gl_FrontMaterial.emission)
					+ specularColor * specular;
				pointLight *= attenuation;
				outSpecular += attenuation * (specular * length(specularColor.rgb));
			}

			vec3 flight = normalize(normalMapLight);//�ʍ��W�n�̌��̕���
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
			�e�ڂ₩��
		*/
		float ChebyshevUpperBound(vec4 shadow_coord, vec4 depth_tex_coord)
		{
			float distance = shadow_coord.z;//���ꂪ�`����W
			//distance = distance * 0.5 + 0.5;// This is done via a bias matrix in main.c
			//*0.5+0.5�̃o�C�A�X�s�񂩂��Ă���̂�0-1�ɂȂ��Ă�

			//distance���`�悵�悤�Ƃ���s�N�Z���̌������_����̐[�x
			//moments.x���V���h�E�}�b�v�̐[�x
			//distance�̕���������(=�����ɋ������߂�)�ꍇ�͓��� 


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
			float variance = moments.y - (moments.x*moments.x);//moments.y��moment2 += 0.25*(dx*dx + dy*dy)����moments.x�̂Q���葽��
			variance = min(max(variance, 0.000002), 1.0);

			float d = distance - moments.x;//�����Ɠ��A�̋������߂��قǂP�i�����j�ɋ߂Â�
			float p_max = variance / (variance + d*d);//variance(�G�b�W)���傫����Α傫���ق�d�̎x�z�͏��Ȃ��Ȃ�B

			return p_max;
		}

		/*
		* �e�����̂��߂̌W��(�e�̂���Ƃ����1, ����ȊO��0)
		* @return �e�W��(�e�̂���Ƃ����1, ����ȊO��0)
		*/
		float ShadowCoef(vec4 shadow_coord, vec4 depth_tex_coord)
		{
			// ��������̃f�v�X�l(���_)
			float view_d = shadow_coord.z;//-0.0001;
			
			// �i�[���ꂽ��������̍ŏ��f�v�X�l���擾
			float light_d = depth_tex_coord.x;

			// �e��0,������1
			float shadow_coef = 1.0;
			if (vShadowCoord.w > 0.0){
				shadow_coef = light_d < view_d ? 0.0 : 1.0;
			}

			return shadow_coef;
		}

		void main(void)
		{
			vec4 texColor = (isUseColorTexture) ? (texture2D(texAlbedo, gl_TexCoord[0].st)) : (gl_Color); //�e����Ȃ����ʂ̃e�N�X�`��
			//vec4 light_col = PhongShading(texColor);	// �\�ʔ��ːF
			vec4 light_col = ApplyMap(texColor);

			// �������W�ɂ����镨�̂̈ʒu
			vec4 shadow_coord = vShadowCoord / vShadowCoord.w;//���ꂪ�`�悵�悤�Ƃ��Ă�����W			

			//// �������W�ɂ����镨�̂̈ʒu�ɑ�������e�e�N�X�`���̒l
			//vec4 depth_tex_coord = texture2D(depth_tex, shadow_coord.xy);
			//// �e�e���W��
			//float shadow_coef = ShadowCoef(shadow_coord, depth_tex_coord);
			//gl_FragColor = (1.0-shadow_ambient)*shadow_coef*light_col
			//				+ (shadow_ambient) * light_col;//���A�̕������F���o��悤��
			//gl_FragColor.w = texColor.w;//�A���t�@�l�͉��Z�̑ΏۊO

			//�e�ڂ₩��
			vec4 depth_tex_coord2 = texture2D(depth_tex2, shadow_coord.xy);//�ڂ����v�f�������Ă�e�e�N�X�`��
			float shadow_blurry = ChebyshevUpperBound(shadow_coord, depth_tex_coord2);
			float shadow_blurry_modify = min(shadow_ambient + shadow_blurry, 1.0);
			vec4 fragColor = vec4(shadow_blurry_modify)*light_col;

			//�A���t�@�l�͌ʂŎZ�o
			//glColor4f()�̃A���t�@�l������K�p����B�t�F�[�h�A�E�g�p�Ȃ�
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