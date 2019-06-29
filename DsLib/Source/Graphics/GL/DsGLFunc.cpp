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
	typedef void   (* DS_PFNGLGETPROGRAMINFOLOGPROC) (GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
	typedef void   (* DS_PFNGLGETPROGRAMIVPROC) (GLuint program, GLenum pname, GLint* param);
	typedef void   (* DS_PFNGLSHADERSOURCEPROC) (GLuint shader, GLsizei count, const GLchar *const* string, const GLint* length);
	typedef void   (* DS_PFNGLCOMPILESHADERPROC) (GLuint shader);
	typedef void   (* DS_PFNGLDELETESHADERPROC) (GLuint shader);
	typedef GLuint (* DS_PFNGLCREATESHADERPROC) (GLenum type);
	typedef GLuint (* DS_PFNGLCREATEPROGRAMPROC) (void);
	typedef void   (* DS_PFNGLATTACHSHADERPROC) (GLuint program, GLuint shader);
	typedef void   (* DS_PFNGLLINKPROGRAMPROC) (GLuint program);
	typedef void   (* DS_PFNGLUSEPROGRAMPROC) (GLuint program);
	typedef GLint  (* DS_PFNGLGETUNIFORMLOCATIONPROC) (GLuint program, const GLchar* name);
	typedef GLint  (* DS_PFNGLGETATTRIBLOCATIONPROC) (GLuint program, const GLchar* name);
	typedef void   (* DS_PFNGLUNIFORM1IPROC) (GLint location, GLint v0);
	typedef void   (* DS_PFNGLUNIFORM1FPROC) (GLint location, GLfloat v0);
	typedef void   (* DS_PFNGLUNIFORM2IPROC) (GLint location, GLint v0, GLint v1);
	typedef void   (* DS_PFNGLUNIFORM2FPROC) (GLint location, GLfloat v0, GLfloat v1);
	typedef void   (* DS_PFNGLUNIFORM1FVPROC) (GLint location, GLsizei count, const GLfloat *value);
	typedef void   (* DS_PFNGLUNIFORM2FVPROC) (GLint location, GLsizei count, const GLfloat *value);
	typedef void   (* DS_PFNGLUNIFORM3FVPROC) (GLint location, GLsizei count, const GLfloat *value);
	typedef void   (* DS_PFNGLUNIFORM4FVPROC) (GLint location, GLsizei count, const GLfloat *value);
	typedef void   (* DS_PFNGLUNIFORM1IVPROC) (GLint location, GLsizei count, const GLint *value);
	typedef void   (* DS_PFNGLUNIFORM2IVPROC) (GLint location, GLsizei count, const GLint *value);
	typedef void   (* DS_PFNGLUNIFORM3IVPROC) (GLint location, GLsizei count, const GLint *value);
	typedef void   (* DS_PFNGLUNIFORM4IVPROC) (GLint location, GLsizei count, const GLint *value);
	typedef void   (* DS_PFNGLUNIFORMMATRIX2FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
	typedef void   (* DS_PFNGLUNIFORMMATRIX3FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
	typedef void   (* DS_PFNGLUNIFORMMATRIX4FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
	typedef void   (* DS_PFNGLACTIVETEXTUREPROC) (GLenum texture);
	typedef void   (* DS_PFNGLGENERATEMIPMAPEXTPROC) (GLenum target);
	typedef void   (* DS_PFNGLGENFRAMEBUFFERSEXTPROC) (GLsizei n, GLuint* framebuffers);
	typedef void   (* DS_PFNGLBINDFRAMEBUFFEREXTPROC) (GLenum target, GLuint framebuffer);
	typedef void   (* DS_PFNGLFRAMEBUFFERTEXTURE2DEXTPROC) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
	typedef GLenum (* DS_PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC) (GLenum target);
	typedef void   (* DS_PFNGLGENRENDERBUFFERSEXTPROC) (GLsizei n, GLuint* renderbuffers);
	typedef void   (* DS_PFNGLBINDRENDERBUFFEREXTPROC) (GLenum target, GLuint renderbuffer);
	typedef void   (* DS_PFNGLRENDERBUFFERSTORAGEEXTPROC) (GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
	typedef void   (* DS_PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC) (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
	typedef void   (* DS_PFNGLDELETEFRAMEBUFFERSEXTPROC) (GLsizei n, const GLuint* framebuffers);
	typedef void   (* DS_PFNGLDELETERENDERBUFFERSEXTPROC) (GLsizei n, const GLuint* renderbuffers);

	DS_PFNGLGETSHADERIVPROC s_glGetShaderiv;
	DS_PFNGLGETSHADERINFOLOGPROC s_glGetShaderInfoLog;
	DS_PFNGLGETPROGRAMINFOLOGPROC s_glGetProgramInfoLog;
	DS_PFNGLGETPROGRAMIVPROC s_glGetProgramiv;
	DS_PFNGLSHADERSOURCEPROC s_glShaderSource;
	DS_PFNGLCOMPILESHADERPROC s_glCompileShader;
	DS_PFNGLDELETESHADERPROC s_glDeleteShader;
	DS_PFNGLCREATESHADERPROC s_glCreateShader;
	DS_PFNGLCREATEPROGRAMPROC s_glCreateProgram;
	DS_PFNGLATTACHSHADERPROC s_glAttachShader;
	DS_PFNGLLINKPROGRAMPROC s_glLinkProgram;
	DS_PFNGLUSEPROGRAMPROC s_glUseProgram;
	DS_PFNGLGETUNIFORMLOCATIONPROC s_glGetUniformLocation;
	DS_PFNGLGETATTRIBLOCATIONPROC s_glGetAttribLocation;
	DS_PFNGLUNIFORM1IPROC s_glUniform1i;
	DS_PFNGLUNIFORM1FPROC s_glUniform1f;
	DS_PFNGLUNIFORM2IPROC s_glUniform2i;
	DS_PFNGLUNIFORM2FPROC s_glUniform2f;
	DS_PFNGLUNIFORM1FVPROC s_glUniform1fv;
	DS_PFNGLUNIFORM2FVPROC s_glUniform2fv;
	DS_PFNGLUNIFORM3FVPROC s_glUniform3fv;
	DS_PFNGLUNIFORM4FVPROC s_glUniform4fv;
	DS_PFNGLUNIFORM1IVPROC s_glUniform1iv;
	DS_PFNGLUNIFORM2IVPROC s_glUniform2iv;
	DS_PFNGLUNIFORM3IVPROC s_glUniform3iv;
	DS_PFNGLUNIFORM4IVPROC s_glUniform4iv;
	DS_PFNGLUNIFORMMATRIX2FVPROC s_glUniformMatrix2fv;
	DS_PFNGLUNIFORMMATRIX3FVPROC s_glUniformMatrix3fv;
	DS_PFNGLUNIFORMMATRIX4FVPROC s_glUniformMatrix4fv;
	DS_PFNGLACTIVETEXTUREPROC s_glActiveTexture;
	DS_PFNGLGENERATEMIPMAPEXTPROC s_glGenerateMipmap;
	DS_PFNGLGENFRAMEBUFFERSEXTPROC s_glGenFramebuffers;
	DS_PFNGLBINDFRAMEBUFFEREXTPROC s_glBindFramebuffer;
	DS_PFNGLFRAMEBUFFERTEXTURE2DEXTPROC s_glFramebufferTexture2D;
	DS_PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC s_glCheckFramebufferStatus;
	DS_PFNGLGENRENDERBUFFERSEXTPROC s_glGenRenderbuffers;
	DS_PFNGLBINDRENDERBUFFEREXTPROC s_glBindRenderbuffer;
	DS_PFNGLRENDERBUFFERSTORAGEEXTPROC s_glRenderbufferStorage;
	DS_PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC s_glFramebufferRenderbuffer;
	DS_PFNGLDELETEFRAMEBUFFERSEXTPROC s_glDeleteFramebuffers;
	DS_PFNGLDELETERENDERBUFFERSEXTPROC s_glDeleteRenderbuffers;
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
	s_glGetProgramInfoLog = (DS_PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");
	if (!s_glGetProgramInfoLog) {
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
	s_glCreateProgram = (DS_PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
	if (!s_glCreateProgram) {
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
	s_glGetAttribLocation = (DS_PFNGLGETATTRIBLOCATIONPROC)wglGetProcAddress("glGetAttribLocation");
	if (!s_glGetAttribLocation) {
		return false;
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
	s_glUniform1fv = (DS_PFNGLUNIFORM1FVPROC)wglGetProcAddress("glUniform1fv");
	if (!s_glUniform1fv) {
		return false;
	}
	s_glUniform2fv = (DS_PFNGLUNIFORM2FVPROC)wglGetProcAddress("glUniform2fv");
	if (!s_glUniform2fv) {
		return false;
	}
	s_glUniform3fv = (DS_PFNGLUNIFORM3FVPROC)wglGetProcAddress("glUniform3fv");
	if (!s_glUniform3fv) {
		return false;
	}
	s_glUniform4fv = (DS_PFNGLUNIFORM4FVPROC)wglGetProcAddress("glUniform4fv");
	if (!s_glUniform4fv) {
		return false;
	}
	s_glUniform1iv = (DS_PFNGLUNIFORM1IVPROC)wglGetProcAddress("glUniform1iv");
	if (!s_glUniform1iv) {
		return false;
	}
	s_glUniform2iv = (DS_PFNGLUNIFORM2IVPROC)wglGetProcAddress("glUniform2iv");
	if (!s_glUniform2iv) {
		return false;
	}
	s_glUniform3iv = (DS_PFNGLUNIFORM3IVPROC)wglGetProcAddress("glUniform3iv");
	if (!s_glUniform3iv) {
		return false;
	}
	s_glUniform4iv = (DS_PFNGLUNIFORM4IVPROC)wglGetProcAddress("glUniform4iv");
	if (!s_glUniform4iv) {
		return false;
	}
	s_glUniformMatrix2fv = (DS_PFNGLUNIFORMMATRIX2FVPROC)wglGetProcAddress("glUniformMatrix2fv");
	if (!s_glUniformMatrix2fv) {
		return false;
	}
	s_glUniformMatrix3fv = (DS_PFNGLUNIFORMMATRIX3FVPROC)wglGetProcAddress("glUniformMatrix3fv");
	if (!s_glUniformMatrix3fv) {
		return false;
	}
	s_glUniformMatrix4fv = (DS_PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
	if (!s_glUniformMatrix4fv) {
		return false;
	}
	s_glActiveTexture = (DS_PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
	if (!s_glActiveTexture) {
		return false;
	}
	s_glGenerateMipmap = (DS_PFNGLGENERATEMIPMAPEXTPROC)wglGetProcAddress("glGenerateMipmap");
	if (!s_glGenerateMipmap) {
		return false;
	}
	s_glGenFramebuffers = (DS_PFNGLGENFRAMEBUFFERSEXTPROC)wglGetProcAddress("glGenFramebuffers");
	if (!s_glGenFramebuffers) {
		return false;
	}
	s_glBindFramebuffer = (DS_PFNGLBINDFRAMEBUFFEREXTPROC)wglGetProcAddress("glBindFramebuffer");
	if (!s_glBindFramebuffer) {
		return false;
	}
	s_glFramebufferTexture2D = (DS_PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)wglGetProcAddress("glFramebufferTexture2D");
	if (!s_glFramebufferTexture2D) {
		return false;
	}
	s_glCheckFramebufferStatus = (DS_PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)wglGetProcAddress("glCheckFramebufferStatus");
	if (!s_glCheckFramebufferStatus) {
		return false;
	}
	s_glGenRenderbuffers = (DS_PFNGLGENRENDERBUFFERSEXTPROC)wglGetProcAddress("glGenRenderbuffers");
	if (!s_glGenRenderbuffers) {
		return false;
	}
	s_glBindRenderbuffer = (DS_PFNGLBINDRENDERBUFFEREXTPROC)wglGetProcAddress("glBindRenderbuffer");
	if (!s_glBindRenderbuffer) {
		return false;
	}
	s_glRenderbufferStorage = (DS_PFNGLRENDERBUFFERSTORAGEEXTPROC)wglGetProcAddress("glRenderbufferStorage");
	if (!s_glRenderbufferStorage) {
		return false;
	}
	s_glFramebufferRenderbuffer = (DS_PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)wglGetProcAddress("glFramebufferRenderbuffer");
	if (!s_glFramebufferRenderbuffer) {
		return false;
	}
	s_glDeleteFramebuffers = (DS_PFNGLDELETEFRAMEBUFFERSEXTPROC)wglGetProcAddress("glDeleteFramebuffers");
	if (!s_glDeleteFramebuffers) {
		return false;
	}
	s_glDeleteRenderbuffers = (DS_PFNGLDELETERENDERBUFFERSEXTPROC)wglGetProcAddress("glDeleteRenderbuffers");
	if (!s_glDeleteRenderbuffers) {
		return false;
	}

	return true;
}

void DsLib::DsGLGetShaderiv(unsigned int shader, unsigned int pname, int* param)
{
	s_glGetShaderiv(shader, pname, param);
}

void DsLib::DsGLGetShaderInfoLog(unsigned int shader, int bufSize, int* length, char* infoLog)
{
	s_glGetShaderInfoLog(shader, bufSize, length, infoLog);
}
void DsLib::DsGLGetProgramInfoLog(unsigned int program, int bufSize, int* length, char* infoLog)
{
	s_glGetProgramInfoLog(program, bufSize, length, infoLog);
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

unsigned int DsLib::DsGLCreateProgram() {
	return s_glCreateProgram();
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

unsigned int DsLib::DsGLGetUniformLocation(unsigned int program, const char* name)
{
	return s_glGetUniformLocation(program, name);
}

unsigned int DsLib::DsGLGetAttribLocation(unsigned int program, const char* name)
{
	return s_glGetAttribLocation(program, name);
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

void DsLib::DsGLUniform2f(unsigned int location, float v0, float v1)
{
	s_glUniform2f(location, v0, v1);
}

void DsLib::DsGLUniform1fv(unsigned int location, int count, const float *value)
{
	s_glUniform1fv(location, count, value);
}

void DsLib::DsGLUniform2fv(unsigned int location, int count, const float *value)
{
	s_glUniform2fv(location, count, value);
}

void DsLib::DsGLUniform3fv(unsigned int location, int count, const float *value)
{
	s_glUniform3fv(location, count, value);
}

void DsLib::DsGLUniform4fv(unsigned int location, int count, const float *value)
{
	s_glUniform4fv(location, count, value);
}

void DsLib::DsGLUniform1iv(unsigned int location, int count, const int *value)
{
	s_glUniform1iv(location, count, value);
}

void DsLib::DsGLUniform2iv(unsigned int location, int count, const int *value)
{
	s_glUniform2iv(location, count, value);
}

void DsLib::DsGLUniform3iv(unsigned int location, int count, const int *value)
{
	s_glUniform3iv(location, count, value);
}

void DsLib::DsGLUniform4iv(unsigned int location, int count, const int *value)
{
	s_glUniform4iv(location, count, value);
}
void DsLib::DsGLUniformMatrix2fv(unsigned int location, int count, bool transpose, const float *value)
{
	s_glUniformMatrix2fv(location, count, transpose, value);
}
void DsLib::DsGLUniformMatrix3fv(unsigned int location, int count, bool transpose, const float *value)
{
	s_glUniformMatrix3fv(location, count, transpose, value);
}
void DsLib::DsGLUniformMatrix4fv(unsigned int location, int count, bool transpose, const float *value)
{
	s_glUniformMatrix4fv(location, count, transpose, value);
}
void DsLib::DsGLActiveTexture(unsigned int texture)
{
	s_glActiveTexture(texture);
}

void DsLib::DsGLGenerateMipmap(unsigned int target)
{
	s_glGenerateMipmap(target);
}

void DsLib::DsGLGenFramebuffers(int n, unsigned int* framebuffers)
{
	s_glGenFramebuffers(n, framebuffers);
}

void DsLib::DsGLBindFramebuffer(unsigned int target, unsigned int framebuffer)
{
	s_glBindFramebuffer(target, framebuffer);
}

void DsLib::DsGLFramebufferTexture2D(unsigned int target, unsigned int attachment, unsigned int textarget, unsigned int texture, int level)
{
	s_glFramebufferTexture2D(target, attachment, textarget, texture, level);
}

unsigned int DsLib::DsGLCheckFramebufferStatus(unsigned int target)
{
	return s_glCheckFramebufferStatus(target);
}

void DsLib::DsGLGenRenderbuffers(int n, unsigned int* renderbuffers)
{
	s_glGenRenderbuffers(n, renderbuffers);
}

void DsLib::DsGLBindRenderbuffer(unsigned int target, unsigned int renderbuffer)
{
	s_glBindRenderbuffer(target, renderbuffer);
}

void DsLib::DsGLRenderbufferStorage(unsigned int target, unsigned int internalformat, int width, int height)
{
	s_glRenderbufferStorage(target, internalformat, width, height);
}

void DsLib::DsGLFramebufferRenderbuffer(unsigned int target, unsigned int attachment, unsigned int renderbuffertarget, unsigned int renderbuffer)
{
	s_glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
}

void DsLib::DsGLDeleteFramebuffers(int n, const unsigned int* framebuffers)
{
	s_glDeleteFramebuffers(n, framebuffers);
}

void DsLib::DsGLDeleteRenderbuffers(int n, const unsigned int* renderbuffers)
{
	s_glDeleteRenderbuffers(n, renderbuffers);
}