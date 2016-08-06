#include "stdafx.h"
#include "GLLoader.h"

#include <cstdlib>
#include <cstring>
#include <cstddef>


#if !defined(GL_MAJOR_VERSION)
#define GL_MAJOR_VERSION 0x821B
#endif

#if !defined(GL_MINOR_VERSION)
#define GL_MINOR_VERSION 0x821C
#endif

#if !defined(GL_NUM_EXTENSIONS)
#define GL_NUM_EXTENSIONS 0x821D
#endif
typedef void(*GlFunctionPointer)();

static GlFunctionPointer IntGetProcAddress(const char* name)
{
	GlFunctionPointer address = reinterpret_cast<GlFunctionPointer>(wglGetProcAddress(reinterpret_cast<LPCSTR>(name)));
	if (address)
	{
		// Test whether the returned value is a valid error code
		ptrdiff_t errorCode = reinterpret_cast<ptrdiff_t>(address);

		if ((errorCode != -1) && (errorCode != 1) && (errorCode != 2) && (errorCode != 3))
			return address;
	}

	static HMODULE module = NULL;

	if (!module)
		module = GetModuleHandleA("OpenGL32.dll");

	if (module)
		return reinterpret_cast<GlFunctionPointer>(GetProcAddress(module, reinterpret_cast<LPCSTR>(name)));

	return 0;
}

int sfogl_ext_SGIS_texture_edge_clamp = sfogl_LOAD_FAILED;
int sfogl_ext_EXT_texture_edge_clamp = sfogl_LOAD_FAILED;
int sfogl_ext_EXT_blend_minmax = sfogl_LOAD_FAILED;
int sfogl_ext_EXT_blend_subtract = sfogl_LOAD_FAILED;
int sfogl_ext_ARB_multitexture = sfogl_LOAD_FAILED;
int sfogl_ext_EXT_blend_func_separate = sfogl_LOAD_FAILED;
int sfogl_ext_ARB_shading_language_100 = sfogl_LOAD_FAILED;
int sfogl_ext_ARB_shader_objects = sfogl_LOAD_FAILED;
int sfogl_ext_ARB_vertex_shader = sfogl_LOAD_FAILED;
int sfogl_ext_ARB_fragment_shader = sfogl_LOAD_FAILED;
int sfogl_ext_ARB_texture_non_power_of_two = sfogl_LOAD_FAILED;
int sfogl_ext_EXT_blend_equation_separate = sfogl_LOAD_FAILED;
int sfogl_ext_EXT_framebuffer_object = sfogl_LOAD_FAILED;

void (CODEGEN_FUNCPTR *sf_ptrc_glBlendEquationEXT)(GLenum) = NULL;

void (CODEGEN_FUNCPTR *sf_ptrc_glActiveTextureARB)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glClientActiveTextureARB)(GLenum) = NULL;

void (CODEGEN_FUNCPTR *sf_ptrc_glBlendFuncSeparateEXT)(GLenum, GLenum, GLenum, GLenum) = NULL;

void (CODEGEN_FUNCPTR *sf_ptrc_glAttachObjectARB)(GLhandleARB, GLhandleARB) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glCompileShaderARB)(GLhandleARB) = NULL;
GLhandleARB(CODEGEN_FUNCPTR *sf_ptrc_glCreateProgramObjectARB)() = NULL;
GLhandleARB(CODEGEN_FUNCPTR *sf_ptrc_glCreateShaderObjectARB)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glDeleteObjectARB)(GLhandleARB) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glDetachObjectARB)(GLhandleARB, GLhandleARB) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glGetActiveUniformARB)(GLhandleARB, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLcharARB *) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glGetAttachedObjectsARB)(GLhandleARB, GLsizei, GLsizei *, GLhandleARB *) = NULL;
GLhandleARB(CODEGEN_FUNCPTR *sf_ptrc_glGetHandleARB)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glGetInfoLogARB)(GLhandleARB, GLsizei, GLsizei *, GLcharARB *) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glGetObjectParameterfvARB)(GLhandleARB, GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glGetObjectParameterivARB)(GLhandleARB, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glGetShaderSourceARB)(GLhandleARB, GLsizei, GLsizei *, GLcharARB *) = NULL;
GLint(CODEGEN_FUNCPTR *sf_ptrc_glGetUniformLocationARB)(GLhandleARB, const GLcharARB *) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glGetUniformfvARB)(GLhandleARB, GLint, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glGetUniformivARB)(GLhandleARB, GLint, GLint *) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glLinkProgramARB)(GLhandleARB) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glShaderSourceARB)(GLhandleARB, GLsizei, const GLcharARB **, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glUniform1fARB)(GLint, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glUniform1fvARB)(GLint, GLsizei, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glUniform1iARB)(GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glUniform1ivARB)(GLint, GLsizei, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glUniform2fARB)(GLint, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glUniform2fvARB)(GLint, GLsizei, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glUniform2iARB)(GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glUniform2ivARB)(GLint, GLsizei, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glUniform3fARB)(GLint, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glUniform3fvARB)(GLint, GLsizei, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glUniform3iARB)(GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glUniform3ivARB)(GLint, GLsizei, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glUniform4fARB)(GLint, GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glUniform4fvARB)(GLint, GLsizei, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glUniform4iARB)(GLint, GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glUniform4ivARB)(GLint, GLsizei, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glUniformMatrix2fvARB)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glUniformMatrix3fvARB)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glUniformMatrix4fvARB)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glUseProgramObjectARB)(GLhandleARB) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glValidateProgramARB)(GLhandleARB) = NULL;

void (CODEGEN_FUNCPTR *sf_ptrc_glBindAttribLocationARB)(GLhandleARB, GLuint, const GLcharARB *) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glGetActiveAttribARB)(GLhandleARB, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLcharARB *) = NULL;
GLint(CODEGEN_FUNCPTR *sf_ptrc_glGetAttribLocationARB)(GLhandleARB, const GLcharARB *) = NULL;

void (CODEGEN_FUNCPTR *sf_ptrc_glBindFramebufferEXT)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glBindRenderbufferEXT)(GLenum, GLuint) = NULL;
GLenum(CODEGEN_FUNCPTR *sf_ptrc_glCheckFramebufferStatusEXT)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glDeleteFramebuffersEXT)(GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glDeleteRenderbuffersEXT)(GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glFramebufferRenderbufferEXT)(GLenum, GLenum, GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glFramebufferTexture1DEXT)(GLenum, GLenum, GLenum, GLuint, GLint) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glFramebufferTexture2DEXT)(GLenum, GLenum, GLenum, GLuint, GLint) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glFramebufferTexture3DEXT)(GLenum, GLenum, GLenum, GLuint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glGenFramebuffersEXT)(GLsizei, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glGenRenderbuffersEXT)(GLsizei, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glGenerateMipmapEXT)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glGetFramebufferAttachmentParameterivEXT)(GLenum, GLenum, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glGetRenderbufferParameterivEXT)(GLenum, GLenum, GLint *) = NULL;
GLboolean(CODEGEN_FUNCPTR *sf_ptrc_glIsFramebufferEXT)(GLuint) = NULL;
GLboolean(CODEGEN_FUNCPTR *sf_ptrc_glIsRenderbufferEXT)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *sf_ptrc_glRenderbufferStorageEXT)(GLenum, GLenum, GLsizei, GLsizei) = NULL;

void (CODEGEN_FUNCPTR *sf_ptrc_glBlendEquationSeparateEXT)(GLenum, GLenum) = NULL;

namespace GLExtensions {


static int Load_EXT_blend_minmax()
{
	int numFailed = 0;
	sf_ptrc_glBlendEquationEXT = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glBlendEquationEXT");
	if (!sf_ptrc_glBlendEquationEXT) numFailed++;
	return numFailed;
}



static int Load_ARB_multitexture()
{
	int numFailed = 0;
	sf_ptrc_glActiveTextureARB = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glActiveTextureARB");
	if (!sf_ptrc_glActiveTextureARB) numFailed++;
	sf_ptrc_glClientActiveTextureARB = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glClientActiveTextureARB");
	if (!sf_ptrc_glClientActiveTextureARB) numFailed++;
	return numFailed;
}



static int Load_EXT_blend_func_separate()
{
	int numFailed = 0;
	sf_ptrc_glBlendFuncSeparateEXT = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLenum))IntGetProcAddress("glBlendFuncSeparateEXT");
	if (!sf_ptrc_glBlendFuncSeparateEXT) numFailed++;
	return numFailed;
}



static int Load_ARB_shader_objects()
{
	int numFailed = 0;
	sf_ptrc_glAttachObjectARB = (void (CODEGEN_FUNCPTR *)(GLhandleARB, GLhandleARB))IntGetProcAddress("glAttachObjectARB");
	if (!sf_ptrc_glAttachObjectARB) numFailed++;
	sf_ptrc_glCompileShaderARB = (void (CODEGEN_FUNCPTR *)(GLhandleARB))IntGetProcAddress("glCompileShaderARB");
	if (!sf_ptrc_glCompileShaderARB) numFailed++;
	sf_ptrc_glCreateProgramObjectARB = (GLhandleARB(CODEGEN_FUNCPTR *)())IntGetProcAddress("glCreateProgramObjectARB");
	if (!sf_ptrc_glCreateProgramObjectARB) numFailed++;
	sf_ptrc_glCreateShaderObjectARB = (GLhandleARB(CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glCreateShaderObjectARB");
	if (!sf_ptrc_glCreateShaderObjectARB) numFailed++;
	sf_ptrc_glDeleteObjectARB = (void (CODEGEN_FUNCPTR *)(GLhandleARB))IntGetProcAddress("glDeleteObjectARB");
	if (!sf_ptrc_glDeleteObjectARB) numFailed++;
	sf_ptrc_glDetachObjectARB = (void (CODEGEN_FUNCPTR *)(GLhandleARB, GLhandleARB))IntGetProcAddress("glDetachObjectARB");
	if (!sf_ptrc_glDetachObjectARB) numFailed++;
	sf_ptrc_glGetActiveUniformARB = (void (CODEGEN_FUNCPTR *)(GLhandleARB, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLcharARB *))IntGetProcAddress("glGetActiveUniformARB");
	if (!sf_ptrc_glGetActiveUniformARB) numFailed++;
	sf_ptrc_glGetAttachedObjectsARB = (void (CODEGEN_FUNCPTR *)(GLhandleARB, GLsizei, GLsizei *, GLhandleARB *))IntGetProcAddress("glGetAttachedObjectsARB");
	if (!sf_ptrc_glGetAttachedObjectsARB) numFailed++;
	sf_ptrc_glGetHandleARB = (GLhandleARB(CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glGetHandleARB");
	if (!sf_ptrc_glGetHandleARB) numFailed++;
	sf_ptrc_glGetInfoLogARB = (void (CODEGEN_FUNCPTR *)(GLhandleARB, GLsizei, GLsizei *, GLcharARB *))IntGetProcAddress("glGetInfoLogARB");
	if (!sf_ptrc_glGetInfoLogARB) numFailed++;
	sf_ptrc_glGetObjectParameterfvARB = (void (CODEGEN_FUNCPTR *)(GLhandleARB, GLenum, GLfloat *))IntGetProcAddress("glGetObjectParameterfvARB");
	if (!sf_ptrc_glGetObjectParameterfvARB) numFailed++;
	sf_ptrc_glGetObjectParameterivARB = (void (CODEGEN_FUNCPTR *)(GLhandleARB, GLenum, GLint *))IntGetProcAddress("glGetObjectParameterivARB");
	if (!sf_ptrc_glGetObjectParameterivARB) numFailed++;
	sf_ptrc_glGetShaderSourceARB = (void (CODEGEN_FUNCPTR *)(GLhandleARB, GLsizei, GLsizei *, GLcharARB *))IntGetProcAddress("glGetShaderSourceARB");
	if (!sf_ptrc_glGetShaderSourceARB) numFailed++;
	sf_ptrc_glGetUniformLocationARB = (GLint(CODEGEN_FUNCPTR *)(GLhandleARB, const GLcharARB *))IntGetProcAddress("glGetUniformLocationARB");
	if (!sf_ptrc_glGetUniformLocationARB) numFailed++;
	sf_ptrc_glGetUniformfvARB = (void (CODEGEN_FUNCPTR *)(GLhandleARB, GLint, GLfloat *))IntGetProcAddress("glGetUniformfvARB");
	if (!sf_ptrc_glGetUniformfvARB) numFailed++;
	sf_ptrc_glGetUniformivARB = (void (CODEGEN_FUNCPTR *)(GLhandleARB, GLint, GLint *))IntGetProcAddress("glGetUniformivARB");
	if (!sf_ptrc_glGetUniformivARB) numFailed++;
	sf_ptrc_glLinkProgramARB = (void (CODEGEN_FUNCPTR *)(GLhandleARB))IntGetProcAddress("glLinkProgramARB");
	if (!sf_ptrc_glLinkProgramARB) numFailed++;
	sf_ptrc_glShaderSourceARB = (void (CODEGEN_FUNCPTR *)(GLhandleARB, GLsizei, const GLcharARB **, const GLint *))IntGetProcAddress("glShaderSourceARB");
	if (!sf_ptrc_glShaderSourceARB) numFailed++;
	sf_ptrc_glUniform1fARB = (void (CODEGEN_FUNCPTR *)(GLint, GLfloat))IntGetProcAddress("glUniform1fARB");
	if (!sf_ptrc_glUniform1fARB) numFailed++;
	sf_ptrc_glUniform1fvARB = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLfloat *))IntGetProcAddress("glUniform1fvARB");
	if (!sf_ptrc_glUniform1fvARB) numFailed++;
	sf_ptrc_glUniform1iARB = (void (CODEGEN_FUNCPTR *)(GLint, GLint))IntGetProcAddress("glUniform1iARB");
	if (!sf_ptrc_glUniform1iARB) numFailed++;
	sf_ptrc_glUniform1ivARB = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLint *))IntGetProcAddress("glUniform1ivARB");
	if (!sf_ptrc_glUniform1ivARB) numFailed++;
	sf_ptrc_glUniform2fARB = (void (CODEGEN_FUNCPTR *)(GLint, GLfloat, GLfloat))IntGetProcAddress("glUniform2fARB");
	if (!sf_ptrc_glUniform2fARB) numFailed++;
	sf_ptrc_glUniform2fvARB = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLfloat *))IntGetProcAddress("glUniform2fvARB");
	if (!sf_ptrc_glUniform2fvARB) numFailed++;
	sf_ptrc_glUniform2iARB = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint))IntGetProcAddress("glUniform2iARB");
	if (!sf_ptrc_glUniform2iARB) numFailed++;
	sf_ptrc_glUniform2ivARB = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLint *))IntGetProcAddress("glUniform2ivARB");
	if (!sf_ptrc_glUniform2ivARB) numFailed++;
	sf_ptrc_glUniform3fARB = (void (CODEGEN_FUNCPTR *)(GLint, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glUniform3fARB");
	if (!sf_ptrc_glUniform3fARB) numFailed++;
	sf_ptrc_glUniform3fvARB = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLfloat *))IntGetProcAddress("glUniform3fvARB");
	if (!sf_ptrc_glUniform3fvARB) numFailed++;
	sf_ptrc_glUniform3iARB = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint, GLint))IntGetProcAddress("glUniform3iARB");
	if (!sf_ptrc_glUniform3iARB) numFailed++;
	sf_ptrc_glUniform3ivARB = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLint *))IntGetProcAddress("glUniform3ivARB");
	if (!sf_ptrc_glUniform3ivARB) numFailed++;
	sf_ptrc_glUniform4fARB = (void (CODEGEN_FUNCPTR *)(GLint, GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glUniform4fARB");
	if (!sf_ptrc_glUniform4fARB) numFailed++;
	sf_ptrc_glUniform4fvARB = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLfloat *))IntGetProcAddress("glUniform4fvARB");
	if (!sf_ptrc_glUniform4fvARB) numFailed++;
	sf_ptrc_glUniform4iARB = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint, GLint, GLint))IntGetProcAddress("glUniform4iARB");
	if (!sf_ptrc_glUniform4iARB) numFailed++;
	sf_ptrc_glUniform4ivARB = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLint *))IntGetProcAddress("glUniform4ivARB");
	if (!sf_ptrc_glUniform4ivARB) numFailed++;
	sf_ptrc_glUniformMatrix2fvARB = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix2fvARB");
	if (!sf_ptrc_glUniformMatrix2fvARB) numFailed++;
	sf_ptrc_glUniformMatrix3fvARB = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix3fvARB");
	if (!sf_ptrc_glUniformMatrix3fvARB) numFailed++;
	sf_ptrc_glUniformMatrix4fvARB = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix4fvARB");
	if (!sf_ptrc_glUniformMatrix4fvARB) numFailed++;
	sf_ptrc_glUseProgramObjectARB = (void (CODEGEN_FUNCPTR *)(GLhandleARB))IntGetProcAddress("glUseProgramObjectARB");
	if (!sf_ptrc_glUseProgramObjectARB) numFailed++;
	sf_ptrc_glValidateProgramARB = (void (CODEGEN_FUNCPTR *)(GLhandleARB))IntGetProcAddress("glValidateProgramARB");
	if (!sf_ptrc_glValidateProgramARB) numFailed++;
	return numFailed;
}



static int Load_ARB_vertex_shader()
{
	int numFailed = 0;
	sf_ptrc_glBindAttribLocationARB = (void (CODEGEN_FUNCPTR *)(GLhandleARB, GLuint, const GLcharARB *))IntGetProcAddress("glBindAttribLocationARB");
	if (!sf_ptrc_glBindAttribLocationARB) numFailed++;
	sf_ptrc_glGetActiveAttribARB = (void (CODEGEN_FUNCPTR *)(GLhandleARB, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLcharARB *))IntGetProcAddress("glGetActiveAttribARB");
	if (!sf_ptrc_glGetActiveAttribARB) numFailed++;
	sf_ptrc_glGetAttribLocationARB = (GLint(CODEGEN_FUNCPTR *)(GLhandleARB, const GLcharARB *))IntGetProcAddress("glGetAttribLocationARB");
	if (!sf_ptrc_glGetAttribLocationARB) numFailed++;
	return numFailed;
}



static int Load_EXT_blend_equation_separate()
{
	int numFailed = 0;
	sf_ptrc_glBlendEquationSeparateEXT = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum))IntGetProcAddress("glBlendEquationSeparateEXT");
	if (!sf_ptrc_glBlendEquationSeparateEXT) numFailed++;
	return numFailed;
}



static int Load_EXT_framebuffer_object()
{
	int numFailed = 0;
	sf_ptrc_glBindFramebufferEXT = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glBindFramebufferEXT");
	if (!sf_ptrc_glBindFramebufferEXT) numFailed++;
	sf_ptrc_glBindRenderbufferEXT = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glBindRenderbufferEXT");
	if (!sf_ptrc_glBindRenderbufferEXT) numFailed++;
	sf_ptrc_glCheckFramebufferStatusEXT = (GLenum(CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glCheckFramebufferStatusEXT");
	if (!sf_ptrc_glCheckFramebufferStatusEXT) numFailed++;
	sf_ptrc_glDeleteFramebuffersEXT = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *))IntGetProcAddress("glDeleteFramebuffersEXT");
	if (!sf_ptrc_glDeleteFramebuffersEXT) numFailed++;
	sf_ptrc_glDeleteRenderbuffersEXT = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *))IntGetProcAddress("glDeleteRenderbuffersEXT");
	if (!sf_ptrc_glDeleteRenderbuffersEXT) numFailed++;
	sf_ptrc_glFramebufferRenderbufferEXT = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLuint))IntGetProcAddress("glFramebufferRenderbufferEXT");
	if (!sf_ptrc_glFramebufferRenderbufferEXT) numFailed++;
	sf_ptrc_glFramebufferTexture1DEXT = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLuint, GLint))IntGetProcAddress("glFramebufferTexture1DEXT");
	if (!sf_ptrc_glFramebufferTexture1DEXT) numFailed++;
	sf_ptrc_glFramebufferTexture2DEXT = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLuint, GLint))IntGetProcAddress("glFramebufferTexture2DEXT");
	if (!sf_ptrc_glFramebufferTexture2DEXT) numFailed++;
	sf_ptrc_glFramebufferTexture3DEXT = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLuint, GLint, GLint))IntGetProcAddress("glFramebufferTexture3DEXT");
	if (!sf_ptrc_glFramebufferTexture3DEXT) numFailed++;
	sf_ptrc_glGenFramebuffersEXT = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glGenFramebuffersEXT");
	if (!sf_ptrc_glGenFramebuffersEXT) numFailed++;
	sf_ptrc_glGenRenderbuffersEXT = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glGenRenderbuffersEXT");
	if (!sf_ptrc_glGenRenderbuffersEXT) numFailed++;
	sf_ptrc_glGenerateMipmapEXT = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glGenerateMipmapEXT");
	if (!sf_ptrc_glGenerateMipmapEXT) numFailed++;
	sf_ptrc_glGetFramebufferAttachmentParameterivEXT = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLint *))IntGetProcAddress("glGetFramebufferAttachmentParameterivEXT");
	if (!sf_ptrc_glGetFramebufferAttachmentParameterivEXT) numFailed++;
	sf_ptrc_glGetRenderbufferParameterivEXT = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint *))IntGetProcAddress("glGetRenderbufferParameterivEXT");
	if (!sf_ptrc_glGetRenderbufferParameterivEXT) numFailed++;
	sf_ptrc_glIsFramebufferEXT = (GLboolean(CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsFramebufferEXT");
	if (!sf_ptrc_glIsFramebufferEXT) numFailed++;
	sf_ptrc_glIsRenderbufferEXT = (GLboolean(CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsRenderbufferEXT");
	if (!sf_ptrc_glIsRenderbufferEXT) numFailed++;
	sf_ptrc_glRenderbufferStorageEXT = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLsizei, GLsizei))IntGetProcAddress("glRenderbufferStorageEXT");
	if (!sf_ptrc_glRenderbufferStorageEXT) numFailed++;
	return numFailed;
}

static int Load_Version_1_1()
{
	int numFailed = 0;
	return numFailed;
}

typedef int(*PFN_LOADFUNCPOINTERS)();
typedef struct sfogl_StrToExtMap_s
{
	const char *extensionName;
	int *extensionVariable;
	PFN_LOADFUNCPOINTERS LoadExtension;
} sfogl_StrToExtMap;

static sfogl_StrToExtMap ExtensionMap[13] = {
	{ "GL_SGIS_texture_edge_clamp", &sfogl_ext_SGIS_texture_edge_clamp, NULL },
	{ "GL_EXT_texture_edge_clamp", &sfogl_ext_EXT_texture_edge_clamp, NULL },
	{ "GL_EXT_blend_minmax", &sfogl_ext_EXT_blend_minmax, Load_EXT_blend_minmax },
	{ "GL_EXT_blend_subtract", &sfogl_ext_EXT_blend_subtract, NULL },
	{ "GL_ARB_multitexture", &sfogl_ext_ARB_multitexture, Load_ARB_multitexture },
	{ "GL_EXT_blend_func_separate", &sfogl_ext_EXT_blend_func_separate, Load_EXT_blend_func_separate },
	{ "GL_ARB_shading_language_100", &sfogl_ext_ARB_shading_language_100, NULL },
	{ "GL_ARB_shader_objects", &sfogl_ext_ARB_shader_objects, Load_ARB_shader_objects },
	{ "GL_ARB_vertex_shader", &sfogl_ext_ARB_vertex_shader, Load_ARB_vertex_shader },
	{ "GL_ARB_fragment_shader", &sfogl_ext_ARB_fragment_shader, NULL },
	{ "GL_ARB_texture_non_power_of_two", &sfogl_ext_ARB_texture_non_power_of_two, NULL },
	{ "GL_EXT_blend_equation_separate", &sfogl_ext_EXT_blend_equation_separate, Load_EXT_blend_equation_separate },
	{ "GL_EXT_framebuffer_object", &sfogl_ext_EXT_framebuffer_object, Load_EXT_framebuffer_object }
};

static int g_extensionMapSize = 13;

static sfogl_StrToExtMap *FindExtEntry(const char *extensionName)
{
	int loop;
	sfogl_StrToExtMap *currLoc = ExtensionMap;
	for (loop = 0; loop < g_extensionMapSize; ++loop, ++currLoc)
	{
		if (strcmp(extensionName, currLoc->extensionName) == 0)
			return currLoc;
	}

	return NULL;
}

static void ClearExtensionVars()
{
	sfogl_ext_SGIS_texture_edge_clamp = sfogl_LOAD_FAILED;
	sfogl_ext_EXT_texture_edge_clamp = sfogl_LOAD_FAILED;
	sfogl_ext_EXT_blend_minmax = sfogl_LOAD_FAILED;
	sfogl_ext_EXT_blend_subtract = sfogl_LOAD_FAILED;
	sfogl_ext_ARB_multitexture = sfogl_LOAD_FAILED;
	sfogl_ext_EXT_blend_func_separate = sfogl_LOAD_FAILED;
	sfogl_ext_ARB_shading_language_100 = sfogl_LOAD_FAILED;
	sfogl_ext_ARB_shader_objects = sfogl_LOAD_FAILED;
	sfogl_ext_ARB_vertex_shader = sfogl_LOAD_FAILED;
	sfogl_ext_ARB_fragment_shader = sfogl_LOAD_FAILED;
	sfogl_ext_ARB_texture_non_power_of_two = sfogl_LOAD_FAILED;
	sfogl_ext_EXT_blend_equation_separate = sfogl_LOAD_FAILED;
	sfogl_ext_EXT_framebuffer_object = sfogl_LOAD_FAILED;
}


static void LoadExtByName(const char *extensionName)
{
	sfogl_StrToExtMap *entry = NULL;
	entry = FindExtEntry(extensionName);
	if (entry)
	{
		if (entry->LoadExtension)
		{
			int numFailed = entry->LoadExtension();
			if (numFailed == 0)
			{
				*(entry->extensionVariable) = sfogl_LOAD_SUCCEEDED;
			}
			else
			{
				*(entry->extensionVariable) = sfogl_LOAD_SUCCEEDED + numFailed;
			}
		}
		else
		{
			*(entry->extensionVariable) = sfogl_LOAD_SUCCEEDED;
		}
	}
}


static void ProcExtsFromExtString(const char *strExtList)
{
	if (!strExtList)
		strExtList = "";

	size_t iExtListLen = strlen(strExtList);
	const char *strExtListEnd = strExtList + iExtListLen;
	const char *strCurrPos = strExtList;
	char strWorkBuff[256];

	while (*strCurrPos)
	{
		/*Get the extension at our position.*/
		int iStrLen = 0;
		const char *strEndStr = strchr(strCurrPos, ' ');
		int iStop = 0;
		if (strEndStr == NULL)
		{
			strEndStr = strExtListEnd;
			iStop = 1;
		}

		iStrLen = (int)((ptrdiff_t)strEndStr - (ptrdiff_t)strCurrPos);

		if (iStrLen > 255)
			return;

		strncpy(strWorkBuff, strCurrPos, iStrLen);
		strWorkBuff[iStrLen] = '\0';

		LoadExtByName(strWorkBuff);

		strCurrPos = strEndStr + 1;
		if (iStop) break;
	}
}

int sfogl_LoadFunctions()
{
	int numFailed = 0;
	ClearExtensionVars();

	const char* extensionString = NULL;

	if (sfogl_GetMajorVersion() < 3)
	{
		// Try to load the < 3.0 way
		glCheck(extensionString = (const char *)glGetString(GL_EXTENSIONS));

		ProcExtsFromExtString(extensionString);
	}
	else
	{
		// Try to load the >= 3.0 way
		const GLubyte* (CODEGEN_FUNCPTR *glGetStringiFunc)(GLenum, GLuint) = NULL;
		glGetStringiFunc = (const GLubyte* (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glGetStringi");

		if (glGetStringiFunc)
		{
			int numExtensions = 0;
			glCheck(glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions));

			if (numExtensions)
			{
				for (unsigned int i = 0; i < static_cast<unsigned int>(numExtensions); ++i)
				{
					glCheck(extensionString = (const char *)glGetStringiFunc(GL_EXTENSIONS, i));

					ProcExtsFromExtString(extensionString);
				}
			}
		}
	}

	numFailed = Load_Version_1_1();

	if (numFailed == 0)
		return sfogl_LOAD_SUCCEEDED;
	else
		return sfogl_LOAD_SUCCEEDED + numFailed;
}
};
int sfogl_LoadFunctions() { return GLExtensions::sfogl_LoadFunctions(); }

static int g_major_version = 0;
static int g_minor_version = 0;

static void ParseVersionFromString(int *pOutMajor, int *pOutMinor, const char *strVersion)
{
	const char *strDotPos = NULL;
	int iLength = 0;
	char strWorkBuff[10];
	*pOutMinor = 0;
	*pOutMajor = 0;

	strDotPos = strchr(strVersion, '.');
	if (!strDotPos)
		return;

	iLength = (int)((ptrdiff_t)strDotPos - (ptrdiff_t)strVersion);
	strncpy(strWorkBuff, strVersion, iLength);
	strWorkBuff[iLength] = '\0';

	*pOutMajor = atoi(strWorkBuff);
	strDotPos = strchr(strVersion + iLength + 1, ' ');
	if (!strDotPos)
	{
		/*No extra data. Take the whole rest of the string.*/
		strcpy(strWorkBuff, strVersion + iLength + 1);
	}
	else
	{
		/*Copy only up until the space.*/
		int iLengthMinor = (int)((ptrdiff_t)strDotPos - (ptrdiff_t)strVersion);
		iLengthMinor = iLengthMinor - (iLength + 1);
		strncpy(strWorkBuff, strVersion + iLength + 1, iLengthMinor);
		strWorkBuff[iLengthMinor] = '\0';
	}

	*pOutMinor = atoi(strWorkBuff);
}

static void GetGLVersion()
{
	glGetIntegerv(GL_MAJOR_VERSION, &g_major_version);
	glGetIntegerv(GL_MINOR_VERSION, &g_minor_version);

	// Check if we have to retrieve the context version using the legacy method
	if (glGetError() == GL_INVALID_ENUM)
	{
		const char* versionString = NULL;
		glCheck(versionString = (const char*)glGetString(GL_VERSION));
		ParseVersionFromString(&g_major_version, &g_minor_version, versionString);
	}
}

int sfogl_GetMajorVersion()
{
	if (g_major_version == 0)
		GetGLVersion();
	return g_major_version;
}

int sfogl_GetMinorVersion()
{
	if (g_major_version == 0) //Yes, check the major version to get the minor one.
		GetGLVersion();
	return g_minor_version;
}

int sfogl_IsVersionGEQ(int majorVersion, int minorVersion)
{
	if (g_major_version == 0)
		GetGLVersion();

	if (majorVersion > g_major_version) return 0;
	if (majorVersion < g_major_version) return 1;
	if (g_minor_version >= minorVersion) return 1;
	return 0;
}


int sfwgl_ext_EXT_swap_control = sfwgl_LOAD_FAILED;
int sfwgl_ext_ARB_multisample = sfwgl_LOAD_FAILED;
int sfwgl_ext_ARB_pixel_format = sfwgl_LOAD_FAILED;
int sfwgl_ext_ARB_create_context = sfwgl_LOAD_FAILED;
int sfwgl_ext_ARB_create_context_profile = sfwgl_LOAD_FAILED;

int (CODEGEN_FUNCPTR *sf_ptrc_wglGetSwapIntervalEXT)(void) = NULL;
BOOL(CODEGEN_FUNCPTR *sf_ptrc_wglSwapIntervalEXT)(int) = NULL;

BOOL(CODEGEN_FUNCPTR *sf_ptrc_wglChoosePixelFormatARB)(HDC, const int *, const FLOAT *, UINT, int *, UINT *) = NULL;
BOOL(CODEGEN_FUNCPTR *sf_ptrc_wglGetPixelFormatAttribfvARB)(HDC, int, int, UINT, const int *, FLOAT *) = NULL;
BOOL(CODEGEN_FUNCPTR *sf_ptrc_wglGetPixelFormatAttribivARB)(HDC, int, int, UINT, const int *, int *) = NULL;

HGLRC(CODEGEN_FUNCPTR *sf_ptrc_wglCreateContextAttribsARB)(HDC, HGLRC, const int *) = NULL;
namespace WglExtensions {
static const char * (CODEGEN_FUNCPTR *sf_ptrc_wglGetExtensionsStringARB)(HDC) = NULL;

typedef int(*PFN_LOADFUNCPOINTERS)(void);
typedef struct sfwgl_StrToExtMap_s
{
	const char *extensionName;
	int *extensionVariable;
	PFN_LOADFUNCPOINTERS LoadExtension;
} sfwgl_StrToExtMap;

static int Load_EXT_swap_control(void)
{
	int numFailed = 0;
	sf_ptrc_wglGetSwapIntervalEXT = (int (CODEGEN_FUNCPTR *)(void))IntGetProcAddress("wglGetSwapIntervalEXT");
	if (!sf_ptrc_wglGetSwapIntervalEXT) numFailed++;
	sf_ptrc_wglSwapIntervalEXT = (BOOL(CODEGEN_FUNCPTR *)(int))IntGetProcAddress("wglSwapIntervalEXT");
	if (!sf_ptrc_wglSwapIntervalEXT) numFailed++;
	return numFailed;
}



static int Load_ARB_pixel_format(void)
{
	int numFailed = 0;
	sf_ptrc_wglChoosePixelFormatARB = (BOOL(CODEGEN_FUNCPTR *)(HDC, const int *, const FLOAT *, UINT, int *, UINT *))IntGetProcAddress("wglChoosePixelFormatARB");
	if (!sf_ptrc_wglChoosePixelFormatARB) numFailed++;
	sf_ptrc_wglGetPixelFormatAttribfvARB = (BOOL(CODEGEN_FUNCPTR *)(HDC, int, int, UINT, const int *, FLOAT *))IntGetProcAddress("wglGetPixelFormatAttribfvARB");
	if (!sf_ptrc_wglGetPixelFormatAttribfvARB) numFailed++;
	sf_ptrc_wglGetPixelFormatAttribivARB = (BOOL(CODEGEN_FUNCPTR *)(HDC, int, int, UINT, const int *, int *))IntGetProcAddress("wglGetPixelFormatAttribivARB");
	if (!sf_ptrc_wglGetPixelFormatAttribivARB) numFailed++;
	return numFailed;
}



static int Load_ARB_create_context(void)
{
	int numFailed = 0;
	sf_ptrc_wglCreateContextAttribsARB = (HGLRC(CODEGEN_FUNCPTR *)(HDC, HGLRC, const int *))IntGetProcAddress("wglCreateContextAttribsARB");
	if (!sf_ptrc_wglCreateContextAttribsARB) numFailed++;
	return numFailed;
}





	static sfwgl_StrToExtMap ExtensionMap[5] = {
		{ "WGL_EXT_swap_control", &sfwgl_ext_EXT_swap_control, Load_EXT_swap_control },
		{ "WGL_ARB_multisample", &sfwgl_ext_ARB_multisample, NULL },
		{ "WGL_ARB_pixel_format", &sfwgl_ext_ARB_pixel_format, Load_ARB_pixel_format },
		{ "WGL_ARB_create_context", &sfwgl_ext_ARB_create_context, Load_ARB_create_context },
		{ "WGL_ARB_create_context_profile", &sfwgl_ext_ARB_create_context_profile, NULL },
	};

	static int g_extensionMapSize = 5;

	static sfwgl_StrToExtMap *FindExtEntry(const char *extensionName)
	{
		int loop;
		sfwgl_StrToExtMap *currLoc = ExtensionMap;
		for (loop = 0; loop < g_extensionMapSize; ++loop, ++currLoc)
		{
			if (strcmp(extensionName, currLoc->extensionName) == 0)
				return currLoc;
		}

		return NULL;
	}

static void ClearExtensionVars(void)
{
	sfwgl_ext_EXT_swap_control = sfwgl_LOAD_FAILED;
	sfwgl_ext_ARB_multisample = sfwgl_LOAD_FAILED;
	sfwgl_ext_ARB_pixel_format = sfwgl_LOAD_FAILED;
	sfwgl_ext_ARB_create_context = sfwgl_LOAD_FAILED;
	sfwgl_ext_ARB_create_context_profile = sfwgl_LOAD_FAILED;
}


static void LoadExtByName(const char *extensionName)
{
	sfwgl_StrToExtMap *entry = NULL;
	entry = FindExtEntry(extensionName);
	if (entry)
	{
		if (entry->LoadExtension)
		{
			int numFailed = entry->LoadExtension();
			if (numFailed == 0)
			{
				*(entry->extensionVariable) = sfwgl_LOAD_SUCCEEDED;
			}
			else
			{
				*(entry->extensionVariable) = sfwgl_LOAD_SUCCEEDED + numFailed;
			}
		}
		else
		{
			*(entry->extensionVariable) = sfwgl_LOAD_SUCCEEDED;
		}
	}
}


static void ProcExtsFromExtString(const char *strExtList)
{
	size_t iExtListLen = strlen(strExtList);
	const char *strExtListEnd = strExtList + iExtListLen;
	const char *strCurrPos = strExtList;
	char strWorkBuff[256];

	while (*strCurrPos)
	{
		/*Get the extension at our position.*/
		int iStrLen = 0;
		const char *strEndStr = strchr(strCurrPos, ' ');
		int iStop = 0;
		if (strEndStr == NULL)
		{
			strEndStr = strExtListEnd;
			iStop = 1;
		}

		iStrLen = (int)((ptrdiff_t)strEndStr - (ptrdiff_t)strCurrPos);

		if (iStrLen > 255)
			return;

		strncpy(strWorkBuff, strCurrPos, iStrLen);
		strWorkBuff[iStrLen] = '\0';

		LoadExtByName(strWorkBuff);

		strCurrPos = strEndStr + 1;
		if (iStop) break;
	}
}

int sfwgl_LoadFunctions(HDC hdc)
{
	ClearExtensionVars();

	sf_ptrc_wglGetExtensionsStringARB = (const char * (CODEGEN_FUNCPTR *)(HDC))IntGetProcAddress("wglGetExtensionsStringARB");
	if (!sf_ptrc_wglGetExtensionsStringARB) return sfwgl_LOAD_FAILED;

	ProcExtsFromExtString((const char *)sf_ptrc_wglGetExtensionsStringARB(hdc));
	return sfwgl_LOAD_SUCCEEDED;
}
};
int sfwgl_LoadFunctions(HDC hdc) { return WglExtensions::sfwgl_LoadFunctions(hdc); }

