// testwtlView.h : interface of the CTestwtlView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CSFView : public CWindowImpl<CSFView> , public sf::RenderWindow {
	//sf::RenderWindow _window;
	uint32_t _framesPerSecond;
public:
	CSFView() : _framesPerSecond(30) {}
	virtual void OnRender() = 0;
	virtual void OnInit() = 0;
	virtual ~CSFView() {}
	DECLARE_WND_CLASS(NULL)
	BEGIN_MSG_MAP(CSFView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MSG_WM_TIMER(OnTimer)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
	END_MSG_MAP()
	void setFramesPerSecond(uint32_t fps) {
		_framesPerSecond = fps;
		KillTimer(IDR_REFRESHTIMER);
		SetTimer(IDR_REFRESHTIMER, _framesPerSecond/1000);
	}
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
		create(m_hWnd);
		SetTimer(IDR_REFRESHTIMER, _framesPerSecond / 1000);
		return 0;
	}
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		close();
		return 0;
	}
	void OnTimer(UINT_PTR nIDEvent) {
		if(nIDEvent == IDR_REFRESHTIMER) OnRender();
	}
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		OnRender();
		return 0;
	}
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		int cx = LOWORD(lParam);
		int cy = HIWORD(lParam);
		setSize(sf::Vector2u(cx, cy));
		sf::View view = getView();
		view.reset(sf::FloatRect(0, 0, float(cx - 1), float(cy - 1)));
		setView(view);
		//_border.setSize(sf::Vector2f(cx, cy));
		//bHandled = FALSE;
		return 0;
	}
	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) { return 1; }
};
class TextureView : public CSFView {
	sf::Sprite _test;
	sf::Texture _texture;
	std::mutex _textureMutex;
public:
	sf::Texture& getTexture() { return _texture; }
	void OnInit() override {
		_texture.loadFromFile("image1.jpg");
		_test.setTexture(_texture, true);
		_test.setPosition(100, 100);
	}
	void lock() { _textureMutex.lock(); }
	void unlock() { _textureMutex.unlock(); }
	void OnRender() override {
		if (_textureMutex.try_lock()) {
			clear();
			_test.setTexture(_texture, true);
			draw(_test);
			display();
			_textureMutex.unlock();
		}
	}
};

class CTestwtlView : public CWindowImpl<CTestwtlView>,  public sf::RenderTarget
{
	HGLRC m_hRC;			// Handle to RC
	HDC m_hRCDC;
	float m_fAngle;
	size_t _textureID;
	SIZE _imageSize;
	CComCriticalSection _lock;
	sf::Sprite _test;
	bool _active;
	virtual bool activate(bool active) override;
public:
	CTestwtlView();
	DECLARE_WND_CLASS(NULL)
	sf::Vector2u getSize(void) const override;

	BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();
	BEGIN_MSG_MAP(CTestwtlView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
	END_MSG_MAP()
	

	void UpdateTexture(size_t width, size_t height, const uint8_t* data);
	// opengl overrides
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);// { return 1; }

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)


};
