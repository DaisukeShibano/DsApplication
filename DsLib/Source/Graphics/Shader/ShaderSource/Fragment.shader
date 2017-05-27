// �o�[�e�b�N�X�V�F�[�_����󂯎��ϐ�
varying vec4 vPos;
varying vec3 vNrm;
varying vec4 vShadowCoord;


// GL����ݒ肳���萔(uniform)
uniform sampler2D tex;			//!< �ʏ�e�N�X�`��
uniform sampler2D depth_tex;	//!< �f�v�X�l�e�N�X�`��

// �e�̔Z��
uniform float shadow_ambient;

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
	return texColor*(ambient + diffuse) + specular;
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

	return shadow_coef;
}

void main(void)
{
	vec4 texColor = texture2D(tex, gl_TexCoord[0].st); //�e����Ȃ����ʂ̃e�N�X�`��
	vec4 light_col = PhongShading(texColor);	// �\�ʔ��ːF
	float shadow_coef = ShadowCoef();	// �e�e���W��
	
	// �o��
	gl_FragColor = shadow_ambient*shadow_coef*light_col + (1.0 - shadow_ambient)*light_col;
	gl_FragColor.w = texColor.w;//�A���t�@�l�͉��Z�̑ΏۊO
}