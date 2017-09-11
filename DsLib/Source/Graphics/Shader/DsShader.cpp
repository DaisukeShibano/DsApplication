#include "DsPch.h"
#include "Graphics/Shader/DsShader.h"
#include "Config/DsConf.h"
#include "Graphics/Shader/ShaderSource/DsDefaultShader.h"
#include "Graphics/Shader/ShaderSource/DsShadowMapShader.h"
#include "Graphics/Shader/ShaderSource/DsShadowBlurShader.h"

#include "gl/glew.h"
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
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH , &logSize);

		if (logSize > 1)
		{
			char logBuffer[1024];
			glGetShaderInfoLog(shader, 1024, &length, logBuffer);
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
		glGetProgramiv(program, GL_INFO_LOG_LENGTH , &logSize);

		if (logSize > 1)
		{
			char logBuffer[1024];
			glGetProgramInfoLog(program, 1024, &length, logBuffer);
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
		glShaderSource(shader, 1, (GLchar **)&buf, &size);
  
		/* �V�F�[�_�̃R���p�C�� */
		glCompileShader(shader);
		
		GLint compiled;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		_printShaderInfoLog(shader);		/* �R���p�C�����O�̏o�� */
		if (compiled == GL_FALSE)
		{
			DS_ERROR("shader compile error\n");
			glDeleteShader(shader);
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
		virtual void SetShadowBlurParam(DsVec2f s, int ts)override;

	private:
		unsigned int m_currentIdx;
		GLuint m_prog[static_cast<int>(SHADER_TYPE::NUM)];
		
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
		_ShaderSource sources[] =
		{
			{ GetDefaultVertexShader(), GetDefaultFragmentShader() },
			{ GetShadowMapVertexShader(), GetShadowMapFragmentShader() },
			{ GetShadowBlurVertexShader(), GetShadowBlurFragmentShader() },
		};
		const int sourceNum = static_cast<int>(SHADER_TYPE::NUM);
		static_assert(sizeof(sources)/sizeof(sources[0]) == sourceNum, "�V�F�[�_�[�̃\�[�X�̐��������܂���");

		for (int sIdx=0; sIdx < sourceNum; ++sIdx)
		{
			/* �V�F�[�_�I�u�W�F�N�g�̍쐬 */
			const GLuint vtxShader = glCreateShader(GL_VERTEX_SHADER);
			const GLuint frgShader = glCreateShader(GL_FRAGMENT_SHADER);

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
			const GLuint prog = glCreateProgram();

			/* �V�F�[�_�I�u�W�F�N�g�̃V�F�[�_�v���O�����ւ̓o�^ */
			glAttachShader(prog, vtxShader);
			glAttachShader(prog, frgShader);

			/* �V�F�[�_�I�u�W�F�N�g�̍폜 */
			glDeleteShader(vtxShader);
			glDeleteShader(frgShader);

			/* �V�F�[�_�v���O�����̃����N */
			GLint linked;
			glLinkProgram(prog);
			glGetProgramiv(prog, GL_LINK_STATUS, &linked);
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
			glUseProgram(m_prog[idx]);
			glUniform1i(glGetUniformLocation(m_prog[idx], "tex"), 0);//���ʂ̃e�N�X�`���͂O�Ԃ�
			glUniform1i(glGetUniformLocation(m_prog[idx], "depth_tex"), 7);//GLSL��"depth_tex"�Ƃ����ϐ��ɒl���Z�b�g
			glUniform1i(glGetUniformLocation(m_prog[idx], "depth_tex2"), 6);
			glUniform1f(glGetUniformLocation(m_prog[idx], "shadow_ambient"), s_shadowCoef);//GLSL��"shadow_ambient"�Ƃ����ϐ��ɒl���Z�b�g
		}
	}

	//virtual
	void DsShaderImp::DisableShader()
	{
		glUseProgram(0);
	}

	//virtual 
	void DsShaderImp::SetTextureUnit(int unit)
	{
		glUniform1i(glGetUniformLocation(m_prog[m_currentIdx], "tex"), unit);
	}

	//virtual 
	void DsShaderImp::SetUseTexture(bool isUse)
	{
		glUniform1i(glGetUniformLocation(m_prog[m_currentIdx], "isUseColorTexture"), isUse);
	}

	//virtual 
	void DsShaderImp::SetUseLight(bool isUse)
	{
		glUniform1i(glGetUniformLocation(m_prog[m_currentIdx], "isUseLight"), isUse);
	}

	//virtual 
	void DsShaderImp::SetUseShadow(bool isUse)
	{
		const float val = (isUse) ? (s_shadowCoef) : (0.0f);
		glUniform1f(glGetUniformLocation(m_prog[m_currentIdx], "shadow_ambient"), val);
	}

	//virtual
	void DsShaderImp::SetShadowBlurParam(DsVec2f s, int ts)
	{
		glUniform2f(glGetUniformLocation(m_prog[m_currentIdx], "ScaleU"), s.x, s.y);
		glUniform1i(glGetUniformLocation(m_prog[m_currentIdx], "textureSource"), ts);
	}
}

//static
 DsShader& DsShader::Create()
{
	DsShader* ret = new DsShaderImp();
	if( NULL == ret )
	{
		DsError::Panic("DsShader:�������m�ێ��s");
	}
	return *ret;
}
