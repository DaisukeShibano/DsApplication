#include "DsPch.h"
#include "Graphics/Shader/ShaderSource/DsSSRShader.h"

using namespace DsLib;
#define DS_SHADER_STR(str) #str


namespace //SSR
{ 
	/***************************************************
	@brief		�o�[�e�b�N�X�V�F�[�_�[
	***************************************************/
	static const char s_vertex1[] = DS_SHADER_STR(
		void main(void)
		{
			gl_Position = ftransform();
			gl_TexCoord[0] = gl_MultiTexCoord0;
		}
	);
	/***************************************************
	@brief		�t���O�����g�V�F�[�_�[
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
			vec3.x pos���[�x�l������O�Ȃ�false�A�����łȂ��Ȃ�true
			vec3.yz ����s�N�Z�����W
		*/
		vec3 frontOrBack(vec4 pos)
		{
			vec3 ret = vec3(0);
			vec4 clip = projectionTransform * pos;//�N���b�v���W�֕ϊ�
			if (clip.w != 0.0) {
				vec2 hitPix = (clip.xy / clip.w)*0.5 + 0.5;//�s�N�Z���ʒu�֕ϊ�

				if ((0.0 < hitPix.x) && (hitPix.x < 1.0) && (0.0 < hitPix.y) && (hitPix.y < 1.0)) {
					float hitPixDepth = texture2D(depTexOri, hitPix).z;//������W�̐[�x�l�擾
					float rayDepth = clip.z / clip.w;

					//���C�̕������ɂ����true
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
			
			//���C���΂�
			vec3 start = pos;
			float rayLen = 50.0;//���C���΂�����
			vec3 end = start + (reflectDir * rayLen);
			
			float iteration = 1000.0;//�Փ˔����
			float dLen = rayLen / iteration;
			
			float thickness = 0.01;//�Փ˔���̂��߂̋[���I�Ȍ����i�P�ʂ͐[�x�l�Ɠ����j
			
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
			//	//�Q����T��
			//	if (0.5 < result.x) {//����
			//		searchPos = searchPos - searchVec;
			//	}
			//	else {//�\��
			//		//����ŉ��ɂ������Ɋђʂ��Ė����Ȃ�q�b�g�����m��
			//		if (i == 0) {
			//			isHit = false;
			//			break;
			//		}
			//		searchPos = searchPos + searchVec;
			//	}
			//}
			//hitPix.x = result.y;
			//hitPix.y = result.z;

			float iteEnd = rayLen - 0.001;//for���I������덷�̂���
			float len = dLen;
			for (; len < iteEnd; len *= 1.1){
				
				//���`�̃f�v�X�l������Ύg����
				//startClip = startClip + dVecClip;
				//hitPix = startClip.xy;
				//
				//if ((0.0 < hitPix.x) && (hitPix.x < 1.0) && (0.0 < hitPix.y) && (hitPix.y < 1.0))
				//{
				//
				//	float hitDepth = texture2D(depTexOri, hitPix).z;//������W�̐[�x�l�擾
				//	float rayDepth = startClip.z;
				//
				//	//���C�̐[�x�l�ƕ`��[�x�l���r���A���C���d�Ȃ��Ă���΃q�b�g
				//	if ((hitDepth < rayDepth) && (rayDepth < (hitDepth + thickness))) {
				//		isHit = true;
				//		break;
				//	}
				//}

				
				vec4 rayP = vec4(start + (reflectDir * len), 1.0);//������W�̃��[���h���W
				vec4 rayClip = projectionTransform * rayP;//�N���b�v���W�֕ϊ�
				if (rayClip.w != 0.0) {
					hitPix = (rayClip.xy / rayClip.w)*0.5 + 0.5;//�s�N�Z���ʒu�֕ϊ�
				
					if ((0.0 < hitPix.x) && (hitPix.x < 1.0) && (0.0 < hitPix.y) && (hitPix.y < 1.0))
					{
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

			//�����Ō���
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
	@brief		�o�[�e�b�N�X�V�F�[�_�[
	***************************************************/
	static const char s_vertex2[] = DS_SHADER_STR(
		void main(void)
		{
			gl_Position = ftransform();
			gl_TexCoord[0] = gl_MultiTexCoord0;
		}
	);

	/***************************************************
	@brief		�t���O�����g�V�F�[�_�[
	***************************************************/
	static const char s_fragment2[] = DS_SHADER_STR(
		uniform sampler2D colTexEff;//�u���[�K�p
		uniform sampler2D colTexOri;//ssr���O
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
