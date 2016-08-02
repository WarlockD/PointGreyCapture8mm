// CaptureGUIView.h : interface of the CCaptureGUIView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once


class CCaptureGUIView : public CWindowImpl<CCaptureGUIView>, COpenGL<CCaptureGUIView>,  public CIdleHandler
{
	FlyCapture2::Image fly_image;
	//CWindow m_window;
	CContainedWindow m_window;
	sf::RenderWindow* m_target;
	sf::RectangleShape m_border;
	sf::Sprite m_sprite;
	sf::Texture m_texture;
	GLfloat m_width;
	GLfloat m_height;
	GLfloat m_fAspect;
	WNDCLASS m_class;
	GLfloat m_fAngle;
	float fFPS;
public:
	DECLARE_WND_CLASS(NULL)
	std::recursive_mutex m_bmpmutex;
	void setFlyImage(const FlyCapture2::Image& image);

	virtual BOOL OnIdle() override
	{
		m_fAngle += 1; // Add some rotation to the cube
		UpdateFPS();
		RedrawWindow();
		return TRUE;
	}
	CCaptureGUIView() :  m_target(nullptr) {}
	~CCaptureGUIView() { }
	BEGIN_MSG_MAP(CCaptureGUIView)
	//	MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		CHAIN_MSG_MAP(COpenGL<CCaptureGUIView>)
	//	MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	//	MESSAGE_HANDLER(WM_SIZE, OnSize)
		
		//CHAIN_MSG_MAP(COpenGL<CCaptureGUIView>)
	END_MSG_MAP()
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
//	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void OnInit(void);
	void OnRender(void);
	void OnResize(int cx, int cy);
	void UpdateFPS();

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	
};
