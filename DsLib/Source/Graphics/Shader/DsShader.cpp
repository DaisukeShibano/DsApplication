#include "DsPch.h"
#include "Graphics/Shader/DsShader.h"
#include "Config/DsConf.h"
#include "Graphics/Shader/ShaderSource/DsDefaultShader.h"
#include "Graphics/Shader/ShaderSource/DsShadowMapShader.h"
#include "Graphics/Shader/ShaderSource/DsBlurShader.h"
#include "Graphics/Shader/ShaderSource/DsBloomShader.h"
#include "Graphics/Shader/ShaderSource/DsSSAOShader.h"
#include "Graphics/Shader/ShaderSource/DsDepthFieldShader.h"
#ifndef _DS_GL_FUNC_
#include "Graphics/GL/DsGLFunc.h"
#endif
#include <gl/GL.h>

using namespace DsLib;
static const float s_shadowCoef = 0.3f;//�e�̈�Ԃ��炢�Ƃ��B0�Ő^����

namespace
{		
	/* �V�F�[�_�R���p�C���G���[�̏o�� */
	static void _printShaderInfoLog( GLuint shader)
	{
		int logSize;
		int length;

		/* ���O�̒����́A�Ō��NULL�������܂� */
		DsGLGetShaderiv(shader, DS_GL_INFO_LOG_LENGTH , &logSize);

		if (logSize > 1)
		{
			char logBuffer[1024];
			DsGLGetShaderInfoLog(shader, 1024, &length, logBuffer);
			//fprintf(stderr, "Shader Compile Info Log\n%s\n", logBuffer);
			DS_LOG(logBuffer);
		}
	}


	/* �V�F�[�_�����N�G���[�̏o�� */
	static void _printProgramInfoLog( GLuint program )
	{
		int logSize;
		int length;

		/* ���O�̒����́A�Ō��NULL�������܂� */
		DsGLGetProgramiv(program, DS_GL_INFO_LOG_LENGTH , &logSize);

		if (logSize > 1)
		{
			char logBuffer[1024];
			DsGLGetProgramInfoLog(program, 1024, &length, logBuffer);
			//fprintf(stderr, "Shader Link Info Log\n%s\n", logBuffer);
			DS_LOG(logBuffer);
		}
	}


	/*
	 * �V�F�[�_�[�v���O���������[�h���A�R���p�C��
	 */
	static int _loadShader( const GLuint shader, std::string& shaderSrc )
	{
		const void *buf = shaderSrc.data();
		const int size = static_cast<int>(shaderSrc.size());
		
		/* �V�F�[�_�I�u�W�F�N�g�Ƀv���O�������Z�b�g */
		DsGLShaderSource(shader, 1, (char **)&buf, &size);
  
		/* �V�F�[�_�̃R���p�C�� */
		DsGLCompileShader(shader);
		
		GLint compiled;
		DsGLGetShaderiv(shader, DS_GL_COMPILE_STATUS, &compiled);
		_printShaderInfoLog(shader);		/* �R���p�C�����O�̏o�� */
		if (compiled == GL_FALSE)
		{
			DS_ERROR("shader compile error\n");
			DsGLDeleteShader(shader);
			return -1;
		}

		return 0;
	}

	/*****************************************************
	@brief	�V�F�[�_�[�����N���X
	******************************************************/
	class DsShaderImp : public DsShader
	{
	public:
		DsShaderImp();
		virtual ~DsShaderImp();
	public:
		virtual void Initialize() override;
		virtual void EnableShader(SHADER_TYPE sType) override;
		virtual void DisableShader() override;
		virtual void SetTextureUnit(int unit) override;
		virtual void SetUseTexture(bool isUse) override;
		virtual void SetUseLight(bool isUse)override;
		virtual void SetUseShadow(bool isUse)override;
		virtual void SetBlurParam(float s, int ts, const BlurParam& bp )override;
		virtual void SetPostEffectParam(int effTex, int oriTex, int oriDepTex)override;
		virtual void SetUseNormalMap(bool isUse) override;
		virtual void SetTime(float t) override;
		virtual void DepthFieldParam(int depTex, int blurTex) override;


	private:
		unsigned int m_currentIdx;
		GLuint m_prog[static_cast<int>(SHADER_TYPE::NUM)];
		GLuint m_tangent;
	};

	DsShaderImp::DsShaderImp()
	:m_currentIdx(0)
	//,m_prog()
	{
	}

	DsShaderImp::~DsShaderImp()
	{
	}

	//virtual
	void DsShaderImp::Initialize()
	{
		struct _ShaderSource {
			std::string pVertexSource;
			std::string pFragmentSource;
		};

		//���т�SHADER_TYPE�ƑΉ�
		_ShaderSource sources[] =
		{
			{ GetDefaultVertexShader(), GetDefaultFragmentShader() },
			{ GetShadowMapVertexShader(), GetShadowMapFragmentShader() },
			{ GetBlurVertexShader(), GetBlurFragmentShaderH() },
			{ GetBlurVertexShader(), GetBlurFragmentShaderV() },
			{ GetBloomVertexShader1(), GetBloomFragmentShader1() },
			{ GetBloomVertexShader2(), GetBloomFragmentShader2() },
			{ GetSSAOVertexShader(), GetSSAOFragmentShader() },
			{ GetDepthFieldVertexShader1(), GetDepthFieldFragmentShader1() },
			{ GetDepthFieldVertexShader2(), GetDepthFieldFragmentShader2() },
		};
		const int sourceNum = static_cast<int>(SHADER_TYPE::NUM);
		static_assert(sizeof(sources)/sizeof(sources[0]) == sourceNum, "�V�F�[�_�[�̃\�[�X�̐��������܂���");


		{//�o�[�W�����m�F
			const GLubyte *version = glGetString(GL_VERSION);
			const GLubyte *glslVersion = glGetString(DS_GL_SHADING_LANGUAGE_VERSION);
			DS_LOG("\n============GL version==============");
			DS_LOG("GL Version:%s", version);
			DS_LOG("GLSL Version:%s", glslVersion);
			DS_LOG("====================================\n");
		}

		for (int sIdx=0; sIdx < sourceNum; ++sIdx)
		{
			/* �V�F�[�_�I�u�W�F�N�g�̍쐬 */
			const GLuint vtxShader = DsGLCreateShader(DS_GL_VERTEX_SHADER);
			const GLuint frgShader = DsGLCreateShader(DS_GL_FRAGMENT_SHADER);

			/* �o�[�e�b�N�X�V�F�[�_�̃��[�h�ƃR���p�C�� */
			if (_loadShader(vtxShader, sources[sIdx].pVertexSource) < 0)
			{
				return;
			}

			/* �t���O�����g�V�F�[�_�̃��[�h�ƃR���p�C�� */
			if (_loadShader(frgShader, sources[sIdx].pFragmentSource) < 0)
			{
				return;
			}

			/* �v���O�����I�u�W�F�N�g�̍쐬 */
			const GLuint prog = DsGLCreateProgram();

			/* �V�F�[�_�I�u�W�F�N�g�̃V�F�[�_�v���O�����ւ̓o�^ */
			DsGLAttachShader(prog, vtxShader);
			DsGLAttachShader(prog, frgShader);

			/* �V�F�[�_�I�u�W�F�N�g�̍폜 */
			DsGLDeleteShader(vtxShader);
			DsGLDeleteShader(frgShader);

			/* �V�F�[�_�v���O�����̃����N */
			GLint linked;
			DsGLLinkProgram(prog);
			DsGLGetProgramiv(prog, DS_GL_LINK_STATUS, &linked);
			_printProgramInfoLog(prog);
			if (linked == GL_FALSE)
			{
				DS_ERROR("Link error of shader!!\n");
				return;
			}

			m_prog[sIdx] = prog;
		}
	}

	//virtula
	void DsShaderImp::EnableShader(SHADER_TYPE sType)
	{
		if (SHADER_TYPE::NUM > sType ) {
			int idx = static_cast<int>(sType);
			m_currentIdx = idx;
			DsGLUseProgram(m_prog[idx]);
			DsGLUniform1i(DsGLGetUniformLocation(m_prog[idx], "texAlbedo"), 0);//���ʂ̃e�N�X�`���͂O�Ԃ�
			DsGLUniform1i(DsGLGetUniformLocation(m_prog[idx], "texNormal"), 1);
			DsGLUniform1i(DsGLGetUniformLocation(m_prog[idx], "depth_tex"), 7);//GLSL��"depth_tex"�Ƃ����ϐ��ɒl���Z�b�g
			DsGLUniform1i(DsGLGetUniformLocation(m_prog[idx], "depth_tex2"), 6);
			DsGLUniform1f(DsGLGetUniformLocation(m_prog[idx], "shadow_ambient"), s_shadowCoef);//GLSL��"shadow_ambient"�Ƃ����ϐ��ɒl���Z�b�g
		}
	}

	//virtual
	void DsShaderImp::DisableShader()
	{
		DsGLUseProgram(0);
	}

	//virtual 
	void DsShaderImp::SetTextureUnit(int unit)
	{
		DsGLUniform1i(DsGLGetUniformLocation(m_prog[m_currentIdx], "tex"), unit);
	}

	//virtual 
	void DsShaderImp::SetUseTexture(bool isUse)
	{
		DsGLUniform1i(DsGLGetUniformLocation(m_prog[m_currentIdx], "isUseColorTexture"), isUse);
	}

	//virtual 
	void DsShaderImp::SetUseLight(bool isUse)
	{
		DsGLUniform1i(DsGLGetUniformLocation(m_prog[m_currentIdx], "isUseLight"), isUse);
	}

	//virtual 
	void DsShaderImp::SetUseShadow(bool isUse)
	{
		const float val = (isUse) ? (s_shadowCoef) : (0.0f);
		DsGLUniform1f(DsGLGetUniformLocation(m_prog[m_currentIdx], "shadow_ambient"), val);
	}

	//virtual
	void DsShaderImp::SetBlurParam(float s, int ts, const BlurParam& bp)
	{
		DsGLUniform1f(DsGLGetUniformLocation(m_prog[m_currentIdx], "scale"), s);
		DsGLUniform1i(DsGLGetUniformLocation(m_prog[m_currentIdx], "textureSource"), ts);
		DsGLUniform1i(DsGLGetUniformLocation(m_prog[m_currentIdx], "pixNum"), bp.pixNum);
		DsGLUniform1fv(DsGLGetUniformLocation(m_prog[m_currentIdx], "weight"), bp.pixNum, bp.weight);
		DsGLUniform1f(DsGLGetUniformLocation(m_prog[m_currentIdx], "weightSum"), bp.weightSum);
	}

	//virtual
	void DsShaderImp::SetPostEffectParam(int effTex, int oriTex, int oriDepTex)
	{
		DsGLUniform1i(DsGLGetUniformLocation(m_prog[m_currentIdx], "colTexEff"), effTex);
		DsGLUniform1i(DsGLGetUniformLocation(m_prog[m_currentIdx], "colTexOri"), oriTex);
		DsGLUniform1i(DsGLGetUniformLocation(m_prog[m_currentIdx], "depTexOri"), oriDepTex);
	}

	//virtual 
	void DsShaderImp::SetUseNormalMap(bool isUse)
	{
		DsGLUniform1i(DsGLGetUniformLocation(m_prog[m_currentIdx], "isUseNormalMap"), isUse);
	}

	//virtual
	void DsShaderImp::SetTime(float t)
	{
		DsGLUniform1f(DsGLGetUniformLocation(m_prog[m_currentIdx], "time"), t);
	}

	//virtual
	void DsShaderImp::DepthFieldParam(int depTex, int blurTex)
	{
		DsGLUniform1i(DsGLGetUniformLocation(m_prog[m_currentIdx], "depTexEff"), depTex);
		DsGLUniform1i(DsGLGetUniformLocation(m_prog[m_currentIdx], "blurTex"), blurTex);
	}

}

//static
 DsShader& DsShader::Create()
{
	DsShader* ret = new DsShaderImp();
	DS_ASSERT(ret, "�������m�ێ��s");
	return *ret;
}

//static
 DsShader::BlurParam DsShader::GetBlurParam(const int pixNum, float level/* = 1.0f*/)
{
	BlurParam ret;

	DS_ASSERT( pixNum <= sizeof(ret.weight) / sizeof(ret.weight[0]), "�u���[�s�N�Z�������I�[�o�[���Ă��܂�");

	//���͂������Q�l
	//https://wgld.org/d/webgl/w057.html
	ret.pixNum = pixNum;
	ret.weightSum = 0.0;
	for (int idx = 0; idx < pixNum; ++idx) {
		float r = 1.0f + 2.0f * fabsf( static_cast<float>(idx - 9) );
		float d = level*100.0f;
		float w = exp(-0.5f*(r*r) / d);
		ret.weight[idx] = w;
		ret.weightSum += w;
	}
	return ret;
}