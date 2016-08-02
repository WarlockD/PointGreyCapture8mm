// stdafx.cpp : source file that includes just the standard includes
//	CaptureGUI.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#if (_ATL_VER < 0x0700)
#include <atlimpl.cpp>
#endif //(_ATL_VER < 0x0700)

#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"jpeg.lib")
#pragma comment(lib,"freetype.lib")


#ifdef _DEBUG
#pragma comment(lib,"FlyCapture2d_v140.lib")
#pragma comment(lib,"opencv_world310d.lib")
#pragma comment(lib,"sfml-main-d.lib")
#pragma comment(lib,"sfml-system-d.lib")
#pragma comment(lib,"sfml-window-d.lib")
#pragma comment(lib,"sfml-graphics-d.lib")

#else
#pragma comment(lib,"FlyCapture2_v140.lib")
#pragma comment(lib,"opencv_world310.lib")
#endif

bool OutputGLError(const char* pszLabel)
{
	GLenum errorno = glGetError();

	if (errorno != GL_NO_ERROR)
	{
		char msg[256];
		sprintf(msg,
			"%s had error: #(%d) %s\r\n",
			pszLabel,
			errorno,
			gluErrorString(errorno));
		MessageBox(NULL, CA2W(msg), L"OpenGLError", MB_OK);
		return true;
	}
	return false;
}
