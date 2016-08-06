// testwtlView.cpp : implementation of the CTestwtlView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "testwtlView.h"


#include <cassert>
namespace sf
{
	////////////////////////////////////////////////////////////
	// We cannot use the default constructor here, because it accesses BlendAlpha, which is also global (and dynamically
	// initialized). Initialization order of global objects in different translation units is not defined.
	const RenderStates RenderStates::Default(BlendMode(
		BlendMode::SrcAlpha, BlendMode::OneMinusSrcAlpha, BlendMode::Add,
		BlendMode::One, BlendMode::OneMinusSrcAlpha, BlendMode::Add));


}; // hack

CTestwtlView::CTestwtlView() : _imageSize{ 0,0 } {}

BOOL CTestwtlView::OnIdle()
{
	m_fAngle += 1; // Add some rotation to the cube
	RedrawWindow();
	return FALSE;
}

 bool CTestwtlView::activate(bool active) {
	 if (_active == active) return true;
	 if (active)
		 ::wglMakeCurrent(GetDC(), m_hRC);
	 else
		 ::wglMakeCurrent(GetDC(), NULL);
	 _active = active;
	 return true;
}
 sf::Vector2u CTestwtlView::getSize(void) const {
	 RECT rect;
	 GetClientRect(&rect);

	 return sf::Vector2u(rect.right - rect.left, rect.bottom - rect.top);
 }
LRESULT CTestwtlView::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	_active = false;
	
	m_hRCDC = GetDC();
	// Fill in the pixel format descriptor.
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;
	
	int nPixelFormat = ::ChoosePixelFormat(m_hRCDC, &pfd);
	ATLASSERT(nPixelFormat != 0);

	BOOL bResult = ::SetPixelFormat(m_hRCDC, nPixelFormat, &pfd);
	ATLASSERT(bResult);

	m_hRC = ::wglCreateContext(m_hRCDC);
	ATLASSERT(m_hRC);

	RenderTarget::initialize();
	RenderTarget::resetGLStates();


	return 0;
}

LRESULT CTestwtlView::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_hRC) {
		::wglDeleteContext(m_hRC);
		m_hRC = NULL;
		ReleaseDC(m_hRCDC);
		//::DeleteDC(m_hRCDC);
		m_hRCDC = NULL;
		
	}
	return 0;
}

LRESULT CTestwtlView::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	RenderTarget::draw(_test);
	::SwapBuffers(m_hRCDC);

	return 0;
}

LRESULT CTestwtlView::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int cx = LOWORD(lParam);
	int cy = HIWORD(lParam);

	if (cx == 0 || cy == 0 || m_hRC == NULL)
		return 0;
	
	RECT rc;
	GetWindowRect(&rc);
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;
	sf::View view = getView();
	view.reset(sf::FloatRect(0, 0, float(width - 1), float(height - 1)));
	setView(view);

	bHandled = FALSE;
	return 0;
}

LRESULT CTestwtlView::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 1;
}
void CTestwtlView::UpdateTexture(size_t width, size_t height, const uint8_t* data) {
	_imageSize.cx = width;
	_imageSize.cy = height;
	CPaintDC dc(*this);
	dc.wglMakeCurrent(m_hRC);

	dc.wglMakeCurrent(NULL);
}

BOOL CTestwtlView::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}

