#ifndef _DS_GL_FUNC_
#define _DS_GL_FUNC_

namespace DsLib
{
	bool DsInitGLFunc();
	void DsGLGetShaderiv(unsigned int shader, unsigned int pname, int* param);
	void DsGlGetShaderInfoLog(unsigned int shader, int bufSize, int* length, char* infoLog);
	void DsGLGetProgramiv(unsigned int program, unsigned int pname, int* param);
	void DsGLShaderSource(unsigned int shader, int count, const char *const* string, const int* length);
	void DsGLCompileShader(unsigned int shader);
	void DsGLDeleteShader(unsigned int shader);
	unsigned int DsGLCreateShader(unsigned int type);
	void DsGLAttachShader(unsigned int program, unsigned int shader);
	void DsGLLinkProgram(unsigned int program);
	void DsGLUseProgram(unsigned int program);
	unsigned int  DsGLGetUniformLocation(unsigned int  program, const char* name);
	void DsGLUniform1i(unsigned int location, int v0);
	void DsGLUniform1f(unsigned int location, float v0);
	void DsGLUniform2i(unsigned int location, int v0, int v1);
	void DsGLUniform2f(unsigned int location, float v0, int v1);


}

#endif