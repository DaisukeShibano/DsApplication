#include "DsPch.h"
#ifndef _DS_GL_FUNC_
#include "Graphics/GL/DsGLFunc.h"
#endif

#include <windows.h>
//#include "gl/glew.h"
#include "gl/GL.h"
#include "gl/GLU.h"

using namespace DsLib;

namespace 
{
	typedef char GLchar;

	typedef void   (* DS_PFNGLGETSHADERIVPROC) (GLuint shader, GLenum pname, GLint* param);
	typedef void   (* DS_PFNGLGETSHADERINFOLOGPROC) (GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
	typedef void   (* DS_PFNGLGETPROGRAMIVPROC) (GLuint program, GLenum pname, GLint* param);
	typedef void   (* DS_PFNGLSHADERSOURCEPROC) (GLuint shader, GLsizei count, const GLchar *const* string, const GLint* length);
	typedef void   (* DS_PFNGLCOMPILESHADERPROC) (GLuint shader);
	typedef void   (* DS_PFNGLDELETESHADERPROC) (GLuint shader);
	typedef GLuint (* DS_PFNGLCREATESHADERPROC) (GLenum type);
	typedef void   (* DS_PFNGLATTACHSHADERPROC) (GLuint program, GLuint shader);
	typedef void   (* DS_PFNGLLINKPROGRAMPROC) (GLuint program);
	typedef void   (* DS_PFNGLUSEPROGRAMPROC) (GLuint program);
	typedef GLint  (* DS_PFNGLGETUNIFORMLOCATIONPROC) (GLuint program, const GLchar* name);
	typedef void   (* DS_PFNGLUNIFORM1IPROC) (GLint location, GLint v0);
	typedef void   (* DS_PFNGLUNIFORM1FPROC) (GLint location, GLfloat v0);
	typedef void   (* DS_PFNGLUNIFORM2IPROC) (GLint location, GLint v0, GLint v1);
	typedef void   (* DS_PFNGLUNIFORM2FPROC) (GLint location, GLfloat v0, GLfloat v1);

	DS_PFNGLGETSHADERIVPROC s_glGetShaderiv;
	DS_PFNGLGETSHADERINFOLOGPROC s_glGetShaderInfoLog;
	DS_PFNGLGETPROGRAMIVPROC s_glGetProgramiv;
	DS_PFNGLSHADERSOURCEPROC s_glShaderSource;
	DS_PFNGLCOMPILESHADERPROC s_glCompileShader;
	DS_PFNGLDELETESHADERPROC s_glDeleteShader;
	DS_PFNGLCREATESHADERPROC s_glCreateShader;
	DS_PFNGLATTACHSHADERPROC s_glAttachShader;
	DS_PFNGLLINKPROGRAMPROC s_glLinkProgram;
	DS_PFNGLUSEPROGRAMPROC s_glUseProgram;
	DS_PFNGLGETUNIFORMLOCATIONPROC s_glGetUniformLocation;
	DS_PFNGLUNIFORM1IPROC s_glUniform1i;
	DS_PFNGLUNIFORM1FPROC s_glUniform1f;
	DS_PFNGLUNIFORM2IPROC s_glUniform2i;
	DS_PFNGLUNIFORM2FPROC s_glUniform2f;
}

bool DsLib::DsInitGLFunc()
{
	s_glGetShaderiv = (DS_PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
	if (!s_glGetShaderiv) {
		return false;
	}
	s_glGetShaderInfoLog = (DS_PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
	if (!s_glGetShaderInfoLog) {
		return false;
	}
	s_glGetProgramiv = (DS_PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
	if (!s_glGetProgramiv) {
		return false;
	}
	s_glShaderSource = (DS_PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
	if (!s_glShaderSource) {
		return false;
	}
	s_glCompileShader = (DS_PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
	if (!s_glCompileShader) {
		return false;
	}
	s_glDeleteShader = (DS_PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
	if (!s_glDeleteShader) {
		return false;
	}
	s_glCreateShader = (DS_PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
	if (!s_glCreateShader) {
		return false;
	}
	s_glAttachShader = (DS_PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
	if (!s_glAttachShader) {
		return false;
	}
	s_glLinkProgram = (DS_PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
	if (!s_glLinkProgram) {
		return false;
	}
	s_glUseProgram = (DS_PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
	if (!s_glUseProgram) {
		return false;
	}
	s_glGetUniformLocation = (DS_PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
	if (!s_glGetUniformLocation) {
		return  false;
	}
	s_glUniform1i = (DS_PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
	if (!s_glUniform1i) {
		return false;
	}
	s_glUniform1f = (DS_PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f");
	if (!s_glUniform1f) {
		return false;
	}
	s_glUniform2i = (DS_PFNGLUNIFORM2IPROC)wglGetProcAddress("glUniform2i");
	if (!s_glUniform2i) {
		return false;
	}
	s_glUniform2f = (DS_PFNGLUNIFORM2FPROC)wglGetProcAddress("glUniform2f");
	if (!s_glUniform2f) {
		return false;
	}

	return true;
}

void DsLib::DsGLGetShaderiv(unsigned int shader, unsigned int pname, int* param)
{
	s_glGetShaderiv(shader, pname, param);
}

void DsLib::DsGlGetShaderInfoLog(unsigned int shader, int bufSize, int* length, char* infoLog)
{
	s_glGetShaderInfoLog(shader, bufSize, length, infoLog);
}

void DsLib::DsGLGetProgramiv(unsigned int program, unsigned int pname, int* param)
{
	s_glGetProgramiv(program, pname, param);
}

void DsLib::DsGLShaderSource(unsigned int shader, int count, const char *const* _string, const int* length)
{
	s_glShaderSource(shader, count, _string, length);
}

void DsLib::DsGLCompileShader(unsigned int shader)
{
	s_glCompileShader(shader);
}

void DsLib::DsGLDeleteShader(unsigned int shader)
{
	s_glDeleteShader(shader);
}

unsigned int DsLib::DsGLCreateShader(unsigned int type)
{
	return s_glCreateShader(type);
}

void DsLib::DsGLAttachShader(unsigned int program, unsigned int shader)
{
	s_glAttachShader(program, shader);
}

void DsLib::DsGLLinkProgram(unsigned int program)
{
	s_glLinkProgram(program);
}

void DsLib::DsGLUseProgram(unsigned int program)
{
	s_glUseProgram(program);
}

unsigned int DsLib::DsGLGetUniformLocation(unsigned int  program, const char* name)
{
	return s_glGetUniformLocation(program, name);
}

void DsLib::DsGLUniform1i(unsigned int location, int v0)
{
	s_glUniform1i(location, v0);
}
void DsLib::DsGLUniform1f(unsigned int location, float v0)
{
	s_glUniform1f(location, v0);
}

void DsLib::DsGLUniform2i(unsigned int location, int v0, int v1)
{
	s_glUniform2i(location, v0, v1);
}

void DsLib::DsGLUniform2f(unsigned int location, float v0, int v1)
{
	s_glUniform2f(location, v0, v1);
}