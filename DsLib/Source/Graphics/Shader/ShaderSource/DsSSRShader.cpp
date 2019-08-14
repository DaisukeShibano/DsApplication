#include "DsPch.h"
#include "Graphics/Shader/ShaderSource/DsSSRShader.h"

using namespace DsLib;
#define DS_SHADER_STR(str) #str


namespace
{
	/***************************************************
	@brief		�o�[�e�b�N�X�V�F�[�_�[
	***************************************************/
	static const char s_vertex[] = DS_SHADER_STR(
		void main(void)
		{
			gl_Position = ftransform();
			gl_TexCoord[0] = gl_MultiTexCoord0;
		}
	);
	/***************************************************
	@brief		�t���O�����g�V�F�[�_�[
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

			//���C���΂�
			vec3 start = pos;
			float rayLen = 3.0;//���C���΂�����
			float iteration = 40.0;//�Փ˔����
			float dLen = rayLen / iteration;
			
			float thickness = 0.01;//�Փ˔���̂��߂̋[���I�Ȍ����i�P�ʂ͐[�x�l�Ɠ����j
			
			bool isHit = false;
			vec2 hitPix;
			float iteEnd = rayLen - 0.001;//for���I������덷�̂���
			for (float len = dLen; len < iteEnd; len += dLen){
				vec4 rayP = vec4(start + (reflectDir * len), 1.0);//������W�̃��[���h���W
				vec4 rayClip = projectionTransform * rayP;//�N���b�v���W�֕ϊ�
				if (rayClip.w != 0.0) {
					hitPix = (rayClip.xy / rayClip.w)*0.5 + 0.5;//�s�N�Z���ʒu�֕ϊ�

					if ((0.0 < hitPix.x) && (hitPix.x < 1.0) && (0.0 < hitPix.y) && (hitPix.y < 1.0)) {

						float hitDepth = texture2D(depTexOri, hitPix).z;//������W�̐[�x�l�擾
						float rayDepth = rayClip.z / rayClip.w;
						
						//���C�̐[�x�l�ƕ`��[�x�l���r���A���C���d�Ȃ��Ă���΃q�b�g
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
