#include "stdafx.h"
#include "CaptureGUIView.h"

using namespace sf;
// GLCube()
// Renders a cube.
//
void GLCube(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2)
{
	glBegin(GL_POLYGON);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(x2, y2, z2);
	glVertex3f(x1, y2, z2);
	glVertex3f(x1, y1, z2);
	glVertex3f(x2, y1, z2);
	glEnd();

	glBegin(GL_POLYGON);
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(x2, y2, z1);
	glVertex3f(x2, y1, z1);
	glVertex3f(x1, y1, z1);
	glVertex3f(x1, y2, z1);
	glEnd();

	glBegin(GL_POLYGON);
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(x1, y2, z2);
	glVertex3f(x1, y2, z1);
	glVertex3f(x1, y1, z1);
	glVertex3f(x1, y1, z2);
	glEnd();

	glBegin(GL_POLYGON);
	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(x2, y2, z2);
	glVertex3f(x2, y1, z2);
	glVertex3f(x2, y1, z1);
	glVertex3f(x2, y2, z1);
	glEnd();

	glBegin(GL_POLYGON);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(x1, y2, z1);
	glVertex3f(x1, y2, z2);
	glVertex3f(x2, y2, z2);
	glVertex3f(x2, y2, z1);
	glEnd();

	glBegin(GL_POLYGON);
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(x1, y1, z1);
	glVertex3f(x2, y1, z1);
	glVertex3f(x2, y1, z2);
	glVertex3f(x1, y1, z2);
	glEnd();
}
void CCaptureGUIView::setFlyImage(const FlyCapture2::Image& image) {
	unsigned int rows, cols, stride;
	FlyCapture2::PixelFormat format;
	image.GetDimensions(&rows, &cols, &stride, &format);
	m_bmpmutex.lock();
	auto error = image.Convert(FlyCapture2::PIXEL_FORMAT_BGR, &fly_image);
	m_bmpmutex.unlock();
	assert(error != FlyCapture2::PGRERROR_OK); // error check
}
const uint checkered_board[] = { 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000 };


LRESULT CCaptureGUIView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// register object for message filtering and idle updates
	//CMessageLoop* pLoop = _Module.GetMessageLoop();
	//ATLASSERT(pLoop != NULL);
	//pLoop->AddMessageFilter(this);
	//pLoop->AddIdleHandler(this);

	bHandled = FALSE;
#if 0
	RECT rect;
	assert(this->GetClientRect(&rect));
//	HWND view1 = CreateWindow(TEXT("STATIC"), TEXT("test"), WS_CHILD | WS_VISIBLE , 0, 50, 400, 400, m_hWnd, NULL, NULL, NULL);
//	HWND view1 = m_window.Create(TEXT("STATIC"), m_hWnd, rect, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS);
//	HWND view1 = m_window.Create(m_hWnd, rect,TEXT("TEST") , WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 0, 666);
	assert(view1);
	m_window.Create(*this,rect,NULL, WS_CHILD | WS_VISIBLE)
	m_target = new sf::RenderWindow(view1);
//	m_window.Attach(view1);
	assert(m_texture.loadFromFile("testprog.jpg"));
	m_border.setOutlineColor(sf::Color(0,0,255));
	m_border.setOutlineThickness(10);
	m_border.setPosition(20, 20);
	m_border.setFillColor(sf::Color(0, 0, 0));
	m_border.setSize(sf::Vector2f(rect.right - rect.left, rect.bottom - rect.top));
	m_sprite = sf::Sprite(m_texture);
	m_sprite.setPosition(0, 0);
#endif
	return 0;
}
void CCaptureGUIView::OnInit(void) {
	m_fAngle = 0.0f;

	glClearColor(0.000f, 0.000f, 0.000f, 1.0f); //Background color

												// Activate lighting and a light source
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

	// Define material parameters
	static GLfloat glfMatAmbient[] = { 0.000f, 0.450f, 1.000f, 1.0f };
	static GLfloat glfMatDiffuse[] = { 0.000f, 0.000f, 0.580f, 1.0f };
	static GLfloat glfMatSpecular[] = { 1.000f, 1.000f, 1.000f, 1.0f };
	static GLfloat glfMatEmission[] = { 0.000f, 0.000f, 0.000f, 1.0f };
	static GLfloat fShininess = 128.000f;

	// Set material parameters
	glMaterialfv(GL_FRONT, GL_AMBIENT, glfMatAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, glfMatDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, glfMatSpecular);
	glMaterialfv(GL_FRONT, GL_EMISSION, glfMatEmission);
	glMaterialf(GL_FRONT, GL_SHININESS, fShininess);
}

void CCaptureGUIView::OnRender(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear buffers
	glLoadIdentity(); // Load identity matrix

					  // Add a light source
	GLfloat glfLight[] = { -4.0f, 4.0f, 4.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, glfLight);

	// Position and rotate the camera
	glTranslatef(0.0f, 0.0f, -5.0f);
	glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(m_fAngle, 0.0f, 1.0f, 0.0f);

	// Draw a cube
	static GLfloat r = .7f;
	GLCube(-r, -r, -r, r, r, r);

	glFlush();
}

void CCaptureGUIView::OnResize(int cx, int cy) {
	GLfloat fFovy = 30.0f; // Field-of-view
	GLfloat fZNear = 1.0f;  // Near clipping plane
	GLfloat fZFar = 10.0f;  // Far clipping plane

							// Calculate viewport aspect
	RECT rc;
	GetClientRect(&rc);

	GLfloat fAspect = (GLfloat)(rc.right - rc.left) / (GLfloat)(rc.bottom - rc.top);

	// Define viewport
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fFovy, fAspect, fZNear, fZFar);
	glViewport(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
	glMatrixMode(GL_MODELVIEW);
}

void CCaptureGUIView::UpdateFPS()
{
	const int FRAMEINTERVAL = 1000;            // Show FPS every 1000th milliseconds
	static DWORD nFrames = 0;                  // Number of frames since last update
	static DWORD nLastTick = GetTickCount();   // Time of last update
	DWORD nTick = GetTickCount();              // Current time
	if (nTick - nLastTick >= FRAMEINTERVAL)
	{
		float fFPS = 1000.0f*(GLfloat)nFrames / (GLfloat)(nTick - nLastTick);
		nLastTick = nTick;
		nFrames = 0;
	}
	nFrames++;
}
#if 0
LRESULT CCaptureGUIView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{

	m_target->close();
	delete m_target;
	m_target = nullptr;
	m_window.DestroyWindow();
	return 0;
}
#endif
LRESULT CCaptureGUIView::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//char szFPS[256];
	//sprintf(szFPS, "%.1f FPS", fFPS);
	CPaintDC dc(m_hWnd);
//	dc.wglMakeCurrent(m_hRC);
//	OnRender();
//	dc.wglMakeCurrent(NULL);

	RECT rect;
	GetClientRect(&rect);
	//rect.left = rect.left + 80;
	//rect.bottom = rect.top + 20;
	dc.SetTextColor(RGB(255, 255, 255));
	dc.SetBkColor(RGB(0, 0, 0));
	dc.DrawText(_T("Hello, ATL"), -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	dc.SwapBuffers();
	bHandled = TRUE;
	return 0;
}
#if 0
LRESULT CCaptureGUIView::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_target) {
		int cx = LOWORD(lParam);
		int cy = HIWORD(lParam);
		float fAspect = (GLfloat)(cx) / (GLfloat)(cy);
		auto lastSize = m_target->getSize();
		sf::Vector2u newSize(cx, cy);
		if (cx == 0 || cy == 0 || m_hWnd == NULL) return 1;

		if (wParam != SIZE_MINIMIZED  && lastSize != newSize)
		{
			RECT rect;
			if (this->GetClientRect(&rect)) {
				UINT uFlags = SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE;
				//	uFlags |= SWP_NOREDRAW;
				m_border.setSize(sf::Vector2f(rect.right - rect.left, rect.bottom - rect.top));
				::SetWindowPos(m_target->getSystemHandle(),NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, uFlags);
				m_sprite.setPosition(cx / 2, cy / 2);
			}
			::PostMessage(m_target->getSystemHandle(), uMsg, wParam, lParam);
			//m_window.PostMessageW(uMsg, wParam, lParam);
		


		//	m_window.ResizeClient(cx, cy);
			//MoveWindow()
			
			//m_border.setSize(sf::Vector2f(rect.right - rect.left, rect.bottom - rect.top));
		//	lastSize = m_window->getSize();// Update the last handled size
		//	m_window->setSize(newSize);
		}

		//	m_window->setSize(sf::Vector2u(cx, cy));
		//	m_sprite.setPosition(cx/2, cy/2);
		//m_sprite.setScale(fAspect*cx, fAspect*cy);
		//bHandled = FALSE;
	}

	return 0;
}
#endif