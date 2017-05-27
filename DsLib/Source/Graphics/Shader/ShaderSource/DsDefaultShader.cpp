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
		varying vec4 vPos;
		varying vec3 vNrm;
		varying vec4 vShadowCoord;	//!< �V���h�E�f�v�X�}�b�v�̎Q�Ɨp���W

		void main(void)
		{
			// �t���O�����g�V�F�[�_�ł̌v�Z�p(���f���r���[�ϊ��̂�)
			vPos = gl_ModelViewMatrix*gl_Vertex;			// ���_�ʒu
			vNrm = normalize(gl_NormalMatrix*gl_Normal);	// ���_�@��
			vShadowCoord = gl_TextureMatrix[7] * gl_ModelViewMatrix*gl_Vertex;	// �e�p���W�l(�������S���W)

			// �`��p
			gl_Position = gl_ProjectionMatrix*vPos;	// ���_�ʒu
			gl_FrontColor = gl_Color;				// ���_�F
			gl_TexCoord[0] = gl_MultiTexCoord0;		// ���_�e�N�X�`�����W
		}
	);


	/***************************************************
	@brief		�t���O�����g�V�F�[�_�[
	***************************************************/
	static const char s_fragment[] = DS_SHADER_STR(
		// �o�[�e�b�N�X�V�F�[�_����󂯎��ϐ�
		varying vec4 vPos;
		varying vec3 vNrm;
		varying vec4 vShadowCoord;


		// GL����ݒ肳���萔(uniform)
		uniform sampler2D tex;			//!< �ʏ�e�N�X�`��
		uniform sampler2D depth_tex;	//!< �f�v�X�l�e�N�X�`��

		// �e�̔Z��
		uniform float shadow_ambient;

		// �e�N�X�`���̗L��
		uniform bool isUseColorTexture=true;
		uniform bool isUseLight = true;

		/*!
		* Phong���˃��f���ɂ��V�F�[�f�B���O
		* @return �\�ʔ��ːF
		*/
		vec4 PhongShading(const vec4 texColor)
		{
			vec3 N = normalize(vNrm);			// �@���x�N�g��
			vec3 L = normalize(gl_LightSource[0].position.xyz - vPos.xyz);	// ���C�g�x�N�g��

			// �����̌v�Z
			//  - OpenGL���v�Z�����������x�Ɣ��ˌW���̐�(gl_FrontLightProduct)��p����D
			vec4 ambient = gl_FrontLightProduct[0].ambient;

			// �g�U���˂̌v�Z
			float dcoef = max(dot(L, N), 0.0);
			vec4 diffuse = gl_FrontLightProduct[0].diffuse*dcoef;


			// ���ʔ��˂̌v�Z
			vec4 specular = vec4(0.0);
			if (dcoef > 0.0){
				vec3 V = normalize(-vPos.xyz);		// �����x�N�g��

				// ���˃x�N�g���̌v�Z(�t�H�����˃��f��)
				vec3 R = reflect(-L, N);
				//vec3 R = 2.0*dot(N, L)*N-L;	// reflect�֐���p���Ȃ��ꍇ
				float specularLight = pow(max(dot(R, V), 0.0), gl_FrontMaterial.shininess);

				specular = gl_FrontLightProduct[0].specular*specularLight;
			}
			vec4 baseColor = (isUseColorTexture) ? (texColor) : (gl_Color);
			return (isUseLight) ? (baseColor*(ambient + diffuse) + specular) : (baseColor);
		}

		/*
			�e�ڂ₩��
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
		* �e�����̂��߂̌W��(�e�̂���Ƃ����1, ����ȊO��0)
		* @return �e�W��(�e�̂���Ƃ����1, ����ȊO��0)
		*/
		float ShadowCoef(void)
		{
			// �������W
			vec4 shadow_coord1 = vShadowCoord / vShadowCoord.w;

			// ��������̃f�v�X�l(���_)
			float view_d = shadow_coord1.z;//-0.0001;

			// �i�[���ꂽ��������̍ŏ��f�v�X�l���擾
			float light_d = texture2D(depth_tex, shadow_coord1.xy).x;

			// �e��0,������1
			float shadow_coef = 1.0;
			if (vShadowCoord.w > 0.0){
				shadow_coef = light_d < view_d ? 0.0 : 1.0;
			}

			//�e�ڂ₩��
			//float shadow_blurry = ChebyshevUpperBound(shadow_coord1.z, shadow_coord1);

			return shadow_coef;
		}

		void main(void)
		{
			vec4 texColor = texture2D(tex, gl_TexCoord[0].st); //�e����Ȃ����ʂ̃e�N�X�`��
			vec4 light_col = PhongShading(texColor);	// �\�ʔ��ːF
			float shadow_coef = ShadowCoef();	// �e�e���W��

			// �o��
			gl_FragColor = shadow_ambient*shadow_coef*light_col
							+ (1.0 - shadow_ambient) *  light_col;//���A�̕������F���o��悤��
			gl_FragColor.w = texColor.w;//�A���t�@�l�͉��Z�̑ΏۊO
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