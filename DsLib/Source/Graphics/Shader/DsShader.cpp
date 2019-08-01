#include "DsPch.h"
#include "Graphics/Shader/DsShader.h"
#include "Config/DsConf.h"
#include "Graphics/Shader/ShaderSource/DsDefaultShader.h"
#include "Graphics/Shader/ShaderSource/DsShadowMapShader.h"
#include "Graphics/Shader/ShaderSource/DsBlurShader.h"
#include "Graphics/Shader/ShaderSource/DsBloomShader.h"
#include "Graphics/Shader/ShaderSource/DsSSAOShader.h"
#include "Graphics/Shader/ShaderSource/DsDepthFieldShader.h"
#include "Graphics/Shader/ShaderSource/DsSSRShader.h"
#ifndef _DS_GL_FUNC_
#include "Graphics/GL/DsGLFunc.h"
#endif
#include <gl/GL.h>

using namespace DsLib;
static const float s_shadowCoef = 0.3f;//影の一番くらいとこ。0で真っ黒

namespace
{
	/* シェーダコンパイルエラーの出力 */
	static void _printShaderInfoLog(GLuint shader)
	{
		int logSize;
		int length;

		/* ログの長さは、最後のNULL文字も含む */
		DsGLGetShaderiv(shader, DS_GL_INFO_LOG_LENGTH, &logSize);

		if (logSize > 1)
		{
			char logBuffer[1024];
			DsGLGetShaderInfoLog(shader, 1024, &length, logBuffer);
			//fprintf(stderr, "Shader Compile Info Log\n%s\n", logBuffer);
			DS_LOG(logBuffer);
		}
	}


	/* シェーダリンクエラーの出力 */
	static void _printProgramInfoLog(GLuint program)
	{
		int logSize;
		int length;

		/* ログの長さは、最後のNULL文字も含む */
		DsGLGetProgramiv(program, DS_GL_INFO_LOG_LENGTH, &logSize);

		if (logSize > 1)
		{
			char logBuffer[1024];
			DsGLGetProgramInfoLog(program, 1024, &length, logBuffer);
			//fprintf(stderr, "Shader Link Info Log\n%s\n", logBuffer);
			DS_LOG(logBuffer);
		}
	}


	/*
	 * シェーダープログラムをロードし、コンパイル
	 */
	static int _loadShader(const GLuint shader, std::string& shaderSrc)
	{
		const void *buf = shaderSrc.data();
		const int size = static_cast<int>(shaderSrc.size());

		/* シェーダオブジェクトにプログラムをセット */
		DsGLShaderSource(shader, 1, (char **)&buf, &size);

		/* シェーダのコンパイル */
		DsGLCompileShader(shader);

		GLint compiled;
		DsGLGetShaderiv(shader, DS_GL_COMPILE_STATUS, &compiled);
		_printShaderInfoLog(shader);		/* コンパイルログの出力 */
		if (compiled == GL_FALSE)
		{
			DS_ERROR("shader compile error\n");
			DsGLDeleteShader(shader);
			return -1;
		}

		return 0;
	}

	static bool _compileShader(GLuint& result, std::string& vSrc, std::string& fSrc)
	{
		bool ret = false;
		/* シェーダオブジェクトの作成 */
		const GLuint vtxShader = DsGLCreateShader(DS_GL_VERTEX_SHADER);
		const GLuint frgShader = DsGLCreateShader(DS_GL_FRAGMENT_SHADER);

		/* バーテックスシェーダのロードとコンパイル */
		if (_loadShader(vtxShader, vSrc) < 0)
		{
			return false;
		}

		/* フラグメントシェーダのロードとコンパイル */
		if (_loadShader(frgShader, fSrc) < 0)
		{
			return false;
		}

		/* プログラムオブジェクトの作成 */
		const GLuint prog = DsGLCreateProgram();

		/* シェーダオブジェクトのシェーダプログラムへの登録 */
		DsGLAttachShader(prog, vtxShader);
		DsGLAttachShader(prog, frgShader);

		/* シェーダオブジェクトの削除 */
		DsGLDeleteShader(vtxShader);
		DsGLDeleteShader(frgShader);

		/* シェーダプログラムのリンク */
		GLint linked;
		DsGLLinkProgram(prog);
		DsGLGetProgramiv(prog, DS_GL_LINK_STATUS, &linked);
		_printProgramInfoLog(prog);
		if (linked == GL_FALSE)
		{
			DS_ERROR("Link error of shader!!\n");
			return false;
		}

		result = prog;
		return true;
	}

	static void _versionDisp()
	{
		const GLubyte *version = glGetString(GL_VERSION);
		const GLubyte *glslVersion = glGetString(DS_GL_SHADING_LANGUAGE_VERSION);
		DS_LOG("\n============GL version==============");
		DS_LOG("GL Version:%s", version);
		DS_LOG("GLSL Version:%s", glslVersion);
		DS_LOG("====================================\n");
	}

	/*****************************************************
	@brief	シェーダー実装クラス
	******************************************************/
	class DsShaderImp : public DsShader
	{
	public:
		DsShaderImp();
		virtual ~DsShaderImp();
	public:
		virtual void Initialize() override;
		virtual void Initialize(std::string& vertex, std::string& flagment) override;
		virtual void EnableShader(SHADER_TYPE sType) override;
		virtual void DisableShader() override;
		virtual void SetDrawModelTransform(const float m[16]) override;
		virtual void SetTextureUnit(int unit) override;
		virtual void SetUseTexture(bool isUse) override;
		virtual void SetUseLight(bool isUse)override;
		virtual void SetUseShadow(bool isUse)override;
		virtual void SetBlurParam(float s, int ts, const BlurParam& bp)override;
		virtual void SetPostEffectParam(int effTex, int oriTex, int oriDepTex)override;
		virtual void SetUseNormalMap(bool isUse) override;
		virtual void SetUseWaveNormalMap(bool isUse) override;
		virtual void SetUseSpecularMap(bool isUse) override;
		virtual void SetTime(float t) override;
		virtual void DepthFieldParam(int depTex, int blurTex) override;


	private:
		unsigned int m_currentIdx;
		GLuint m_prog[static_cast<int>(SHADER_TYPE::NUM)];
		unsigned int m_uniform_drawModelTransform;
		unsigned int m_uniform_texAlbedo;
		unsigned int m_uniform_texNormal;
		unsigned int m_uniform_texSpecular;
		unsigned int m_uniform_depth_tex;
		unsigned int m_uniform_depth_tex2;
		unsigned int m_uniform_shadow_ambient;
		unsigned int m_uniform_tex;
		unsigned int m_uniform_isUseColorTexture;
		unsigned int m_uniform_isUseLight;
		unsigned int m_uniform_scale;
		unsigned int m_uniform_textureSource;
		unsigned int m_uniform_pixNum;
		unsigned int m_uniform_weight;
		unsigned int m_uniform_weightSum;
		unsigned int m_uniform_colTexEff;
		unsigned int m_uniform_colTexOri;
		unsigned int m_uniform_depTexOri;
		unsigned int m_uniform_isUseNormalMap;
		unsigned int m_uniform_isUseWaveNormalMap;
		unsigned int m_uniform_isUseSpecularMap;
		unsigned int m_uniform_time;
		unsigned int m_uniform_depTexEff;
		unsigned int m_uniform_blurTex;
		
	};

	DsShaderImp::DsShaderImp()
		: m_currentIdx(0)
		, m_prog{}
		, m_uniform_drawModelTransform(0)
		, m_uniform_texAlbedo(0)
		, m_uniform_texNormal(0)
		, m_uniform_texSpecular(0)
		, m_uniform_depth_tex(0)
		, m_uniform_depth_tex2(0)
		, m_uniform_shadow_ambient(0)
		, m_uniform_tex(0)
		, m_uniform_isUseColorTexture(0)
		, m_uniform_isUseLight(0)
		, m_uniform_scale(0)
		, m_uniform_textureSource(0)
		, m_uniform_pixNum(0)
		, m_uniform_weight(0)
		, m_uniform_weightSum(0)
		, m_uniform_colTexEff(0)
		, m_uniform_colTexOri(0)
		, m_uniform_depTexOri(0)
		, m_uniform_isUseNormalMap(0)
		, m_uniform_isUseWaveNormalMap(0)
		, m_uniform_isUseSpecularMap(0)
		, m_uniform_time(0)
		, m_uniform_depTexEff(0)
		, m_uniform_blurTex(0)
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

		//並びはSHADER_TYPEと対応
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
			{ GetSSRVertexShader(), GetSSRFragmentShader() },
		};
		const int sourceNum = static_cast<int>(SHADER_TYPE::NUM);
		static_assert(sizeof(sources) / sizeof(sources[0]) == sourceNum, "シェーダーのソースの数が合いません");

		_versionDisp();

		for (int sIdx = 0; sIdx < sourceNum; ++sIdx)
		{
			_compileShader(m_prog[sIdx], sources[sIdx].pVertexSource, sources[sIdx].pFragmentSource);
		}
	}

	//virtual
	void DsShaderImp::Initialize(std::string& vertex, std::string& flagment)
	{
		_versionDisp();
		_compileShader(m_prog[0], vertex, flagment);
	}

	//virtula
	void DsShaderImp::EnableShader(SHADER_TYPE sType)
	{
		if (SHADER_TYPE::NUM > sType) {
			int idx = static_cast<int>(sType);
			m_currentIdx = idx;
			DsGLUseProgram(m_prog[idx]);

			//呼び出し回数が多く負荷になってるので最初に求めておく
			m_uniform_drawModelTransform      = DsGLGetUniformLocation(m_prog[idx], "drawModelTransform");
			m_uniform_texAlbedo               = DsGLGetUniformLocation(m_prog[idx], "texAlbedo");
			m_uniform_texNormal               = DsGLGetUniformLocation(m_prog[idx], "texNormal");
			m_uniform_texSpecular			  = DsGLGetUniformLocation(m_prog[idx], "texSpecular");
			m_uniform_depth_tex				  = DsGLGetUniformLocation(m_prog[idx], "depth_tex");
			m_uniform_depth_tex2			  = DsGLGetUniformLocation(m_prog[idx], "depth_tex2");
			m_uniform_shadow_ambient		  = DsGLGetUniformLocation(m_prog[idx], "shadow_ambient");
			m_uniform_tex					  = DsGLGetUniformLocation(m_prog[idx], "tex");
			m_uniform_isUseColorTexture		  = DsGLGetUniformLocation(m_prog[idx], "isUseColorTexture");
			m_uniform_isUseLight			  = DsGLGetUniformLocation(m_prog[idx], "isUseLight");
			m_uniform_scale					  = DsGLGetUniformLocation(m_prog[idx], "scale");
			m_uniform_textureSource			  = DsGLGetUniformLocation(m_prog[idx], "textureSource");
			m_uniform_pixNum				  = DsGLGetUniformLocation(m_prog[idx], "pixNum");
			m_uniform_weight				  = DsGLGetUniformLocation(m_prog[idx], "weight");
			m_uniform_weightSum				  = DsGLGetUniformLocation(m_prog[idx], "weightSum");
			m_uniform_colTexEff				  = DsGLGetUniformLocation(m_prog[idx], "colTexEff");
			m_uniform_colTexOri				  = DsGLGetUniformLocation(m_prog[idx], "colTexOri");
			m_uniform_depTexOri				  = DsGLGetUniformLocation(m_prog[idx], "depTexOri");
			m_uniform_isUseNormalMap		  = DsGLGetUniformLocation(m_prog[idx], "isUseNormalMap");
			m_uniform_isUseWaveNormalMap	  = DsGLGetUniformLocation(m_prog[idx], "isUseWaveNormalMap");
			m_uniform_isUseSpecularMap		  = DsGLGetUniformLocation(m_prog[idx], "isUseSpecularMap");
			m_uniform_time					  = DsGLGetUniformLocation(m_prog[idx], "time");
			m_uniform_depTexEff				  = DsGLGetUniformLocation(m_prog[idx], "depTexEff");
			m_uniform_blurTex				  = DsGLGetUniformLocation(m_prog[idx], "blurTex");

			DsGLUniform1i(m_uniform_texAlbedo, 0);//普通のテクスチャは０番に
			DsGLUniform1i(m_uniform_texNormal, 1);
			DsGLUniform1i(m_uniform_texSpecular, 2);
			DsGLUniform1i(m_uniform_depth_tex, 7);
			DsGLUniform1i(m_uniform_depth_tex2, 6);
			DsGLUniform1f(m_uniform_shadow_ambient, s_shadowCoef);
		}
	}

	//virtual
	void DsShaderImp::DisableShader()
	{
		DsGLUseProgram(0);
	}
	//virtual
	void DsShaderImp::SetDrawModelTransform(const float m[16]) {
		DsGLUniformMatrix4fv(m_uniform_drawModelTransform, 1, false, m);
	}

	//virtual 
	void DsShaderImp::SetTextureUnit(int unit)
	{
		DsGLUniform1i(m_uniform_tex, unit);
	}

	//virtual 
	void DsShaderImp::SetUseTexture(bool isUse)
	{
		DsGLUniform1i(m_uniform_isUseColorTexture, isUse);
	}

	//virtual 
	void DsShaderImp::SetUseLight(bool isUse)
	{
		DsGLUniform1i(m_uniform_isUseLight, isUse);
	}

	//virtual 
	void DsShaderImp::SetUseShadow(bool isUse)
	{
		const float val = (isUse) ? (s_shadowCoef) : (0.0f);
		DsGLUniform1f(m_uniform_shadow_ambient, val);
	}

	//virtual
	void DsShaderImp::SetBlurParam(float s, int ts, const BlurParam& bp)
	{
		DsGLUniform1f(m_uniform_scale, s);
		DsGLUniform1i(m_uniform_textureSource, ts);
		DsGLUniform1i(m_uniform_pixNum, bp.pixNum);
		DsGLUniform1fv(m_uniform_weight, bp.pixNum, bp.weight);
		DsGLUniform1f(m_uniform_weightSum, bp.weightSum);
	}

	//virtual
	void DsShaderImp::SetPostEffectParam(int effTex, int oriTex, int oriDepTex)
	{
		DsGLUniform1i(m_uniform_colTexEff, effTex);
		DsGLUniform1i(m_uniform_colTexOri, oriTex);
		DsGLUniform1i(m_uniform_depTexOri, oriDepTex);
	}

	//virtual 
	void DsShaderImp::SetUseNormalMap(bool isUse)
	{
		DsGLUniform1i(m_uniform_isUseNormalMap, isUse);
	}

	//virtual
	void DsShaderImp::SetUseWaveNormalMap(bool isUse)
	{
		DsGLUniform1i(m_uniform_isUseWaveNormalMap, isUse);
	}

	//virtual 
	void DsShaderImp::SetUseSpecularMap(bool isUse)
	{
		DsGLUniform1i(m_uniform_isUseSpecularMap, isUse);
	}

	//virtual
	void DsShaderImp::SetTime(float t)
	{
		DsGLUniform1f(m_uniform_time, t);
	}

	//virtual
	void DsShaderImp::DepthFieldParam(int depTex, int blurTex)
	{
		DsGLUniform1i(m_uniform_depTexEff, depTex);
		DsGLUniform1i(m_uniform_blurTex, blurTex);
	}

}

//static
 DsShader& DsShader::Create()
{
	DsShader* ret = new DsShaderImp();
	DS_ASSERT(ret, "メモリ確保失敗");
	return *ret;
}

//static
 DsShader::BlurParam DsShader::GetBlurParam(const int pixNum, float level/* = 1.0f*/)
{
	BlurParam ret;

	DS_ASSERT( pixNum <= sizeof(ret.weight) / sizeof(ret.weight[0]), "ブラーピクセル数がオーバーしています");

	//式はここを参考
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