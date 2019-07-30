#ifndef _DS_GL_FUNC_
#define _DS_GL_FUNC_

namespace DsLib
{
	enum
	{
		DS_GL_SHADING_LANGUAGE_VERSION = 0x8B8C,

		DS_GL_TEXTURE0 = 0x84C0,
		DS_GL_TEXTURE1 = 0x84C1,
		DS_GL_TEXTURE2 = 0x84C2,
		DS_GL_TEXTURE3 = 0x84C3,
		DS_GL_TEXTURE4 = 0x84C4,
		DS_GL_TEXTURE5 = 0x84C5,
		DS_GL_TEXTURE6 = 0x84C6,
		DS_GL_TEXTURE7 = 0x84C7,
		DS_GL_TEXTURE8 = 0x84C8,
		DS_GL_TEXTURE9 = 0x84C9,
		DS_GL_TEXTURE10 = 0x84CA,
		DS_GL_TEXTURE11 = 0x84CB,
		DS_GL_TEXTURE12 = 0x84CC,
		DS_GL_TEXTURE13 = 0x84CD,
		DS_GL_TEXTURE14 = 0x84CE,
		DS_GL_TEXTURE15 = 0x84CF,
		DS_GL_TEXTURE16 = 0x84D0,
		DS_GL_TEXTURE17 = 0x84D1,
		DS_GL_TEXTURE18 = 0x84D2,
		DS_GL_TEXTURE19 = 0x84D3,
		DS_GL_TEXTURE20 = 0x84D4,
		DS_GL_TEXTURE21 = 0x84D5,
		DS_GL_TEXTURE22 = 0x84D6,
		DS_GL_TEXTURE23 = 0x84D7,
		DS_GL_TEXTURE24 = 0x84D8,
		DS_GL_TEXTURE25 = 0x84D9,
		DS_GL_TEXTURE26 = 0x84DA,
		DS_GL_TEXTURE27 = 0x84DB,
		DS_GL_TEXTURE28 = 0x84DC,
		DS_GL_TEXTURE29 = 0x84DD,
		DS_GL_TEXTURE30 = 0x84DE,
		DS_GL_TEXTURE31 = 0x84DF,

		DS_GL_INFO_LOG_LENGTH = 0x8B84,
		DS_GL_COMPILE_STATUS = 0x8B81,
		DS_GL_VERTEX_SHADER = 0x8B31,
		DS_GL_FRAGMENT_SHADER = 0x8B30,
		DS_GL_LINK_STATUS = 0x8B82,
		DS_GL_FRAMEBUFFER = 0x8D40,
		DS_GL_DEPTH_ATTACHMENT = 0x8D00,
		DS_GL_COLOR_ATTACHMENT0 = 0x8CE0,
		DS_GL_COLOR_ATTACHMENT1 = 0x8CE1,
		DS_GL_COLOR_ATTACHMENT2 = 0x8CE2,
		DS_GL_COLOR_ATTACHMENT3 = 0x8CE3,
		DS_GL_COLOR_ATTACHMENT4 = 0x8CE4,
		DS_GL_COLOR_ATTACHMENT5 = 0x8CE5,
		DS_GL_COLOR_ATTACHMENT6 = 0x8CE6,
		DS_GL_COLOR_ATTACHMENT7 = 0x8CE7,
		DS_GL_COLOR_ATTACHMENT8 = 0x8CE8,
		DS_GL_COLOR_ATTACHMENT9 = 0x8CE9,
		DS_GL_COLOR_ATTACHMENT10 = 0x8CEA,
		DS_GL_COLOR_ATTACHMENT11 = 0x8CEB,
		DS_GL_COLOR_ATTACHMENT12 = 0x8CEC,
		DS_GL_COLOR_ATTACHMENT13 = 0x8CED,
		DS_GL_COLOR_ATTACHMENT14 = 0x8CEE,
		DS_GL_COLOR_ATTACHMENT15 = 0x8CEF,
		DS_GL_FRAMEBUFFER_COMPLETE = 0x8CD5,
		DS_GL_RENDERBUFFER = 0x8D41,
		DS_GL_DEPTH_COMPONENT16 = 0x81A5,
		DS_GL_DEPTH_COMPONENT24 = 0x81A6,
		DS_GL_DEPTH_COMPONENT32 = 0x81A7,

		DS_GL_RGB32F_ARB = 0x8815,
		DS_GL_RGBA32F = 0x8814,
		DS_GL_RGB32F = 0x8815,
		DS_GL_CLAMP_TO_BORDER = 0x812D,

	};

	bool DsInitGLFunc();
	void DsGLGetShaderiv(unsigned int shader, unsigned int pname, int* param);
	void DsGLGetShaderInfoLog(unsigned int shader, int bufSize, int* length, char* infoLog);
	void DsGLGetProgramInfoLog(unsigned int program, int bufSize, int* length, char* infoLog);
	void DsGLGetProgramiv(unsigned int program, unsigned int pname, int* param);
	void DsGLShaderSource(unsigned int shader, int count, const char *const* string, const int* length);
	void DsGLCompileShader(unsigned int shader);
	void DsGLDeleteShader(unsigned int shader);
	unsigned int DsGLCreateShader(unsigned int type);
	unsigned int DsGLCreateProgram();
	void DsGLAttachShader(unsigned int program, unsigned int shader);
	void DsGLLinkProgram(unsigned int program);
	void DsGLUseProgram(unsigned int program);
	unsigned int DsGLGetUniformLocation(unsigned int program, const char* name);
	unsigned int DsGLGetAttribLocation(unsigned int program, const char* name);
	void DsGLUniform1i(unsigned int location, int v0);
	void DsGLUniform1f(unsigned int location, float v0);
	void DsGLUniform2i(unsigned int location, int v0, int v1);
	void DsGLUniform2f(unsigned int location, float v0, float v1);

	void DsGLUniform1fv(unsigned int location, int count, const float *value);
	void DsGLUniform2fv(unsigned int location, int count, const float *value);
	void DsGLUniform3fv(unsigned int location, int count, const float *value);
	void DsGLUniform4fv(unsigned int location, int count, const float *value);
	void DsGLUniform1iv(unsigned int location, int count, const int *value);
	void DsGLUniform2iv(unsigned int location, int count, const int *value);
	void DsGLUniform3iv(unsigned int location, int count, const int *value);
	void DsGLUniform4iv(unsigned int location, int count, const int *value);
	void DsGLUniformMatrix2fv(unsigned int location, int count, bool transpose, const float *value);
	void DsGLUniformMatrix3fv(unsigned int location, int count, bool transpose, const float *value);
	void DsGLUniformMatrix4fv(unsigned int location, int count, bool transpose, const float *value);

	void DsGLActiveTexture(unsigned int texture);
	void DsGLGenerateMipmap(unsigned int target);
	void DsGLGenFramebuffers(int n, unsigned int* framebuffers);
	void DsGLBindFramebuffer(unsigned int target, unsigned int framebuffer);
	void DsGLFramebufferTexture2D(unsigned int target, unsigned int attachment, unsigned int textarget, unsigned int texture, int level);
	unsigned int DsGLCheckFramebufferStatus(unsigned int target);
	void DsGLGenRenderbuffers(int n, unsigned int* renderbuffers);
	void DsGLBindRenderbuffer(unsigned int target, unsigned int renderbuffer);
	void DsGLRenderbufferStorage(unsigned int target, unsigned int internalformat, int width, int height);
	void DsGLFramebufferRenderbuffer(unsigned int target, unsigned int attachment, unsigned int renderbuffertarget, unsigned int renderbuffer);
	void DsGLDeleteFramebuffers(int n, const unsigned int* framebuffers);
	void DsGLDeleteRenderbuffers(int n, const unsigned int* renderbuffers);
	void DsGLDrawBuffers(int n, const unsigned int* bufs);

}

#endif