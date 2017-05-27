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