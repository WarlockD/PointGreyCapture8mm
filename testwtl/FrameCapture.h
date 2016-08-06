#pragma once

typedef std::function<void(const FlyCapture2::Image&)> ImageUpdateDelegate;
typedef std::function<void(uint32_t, uint32_t, const std::vector<uint8_t>&)> RawTextureDataDelegate;
class FrameCapture {
	std::vector<uint8_t> _rawFrame;
	sf::Vector2u _rawFrameSize;
	FlyCapture2::Image _lastFrame;
	std::thread _captureThread;
	std::mutex _capture_mutex;
	FlyCapture2::Camera _camera;
	FlyCapture2::CameraInfo _camInfo;
	volatile bool _isCaptureing = false;
	volatile bool _stopRequested = false;
	static void capture_thread(FrameCapture* fc);
	HWND _windowHandle;
	ImageUpdateDelegate _frameUpdate;
	RawTextureDataDelegate _rawUpdate;
public:
	FrameCapture::FrameCapture() : _rawFrameSize(0, 0)
	{
		_rawFrame.resize(2048 * 2048 * 4);
	}

	void setImageCallback(ImageUpdateDelegate func) {
		std::lock_guard<std::mutex> lock(_capture_mutex);
		_frameUpdate = func;
	}
	void setRawTextureCallback(RawTextureDataDelegate func) {
		std::lock_guard<std::mutex> lock(_capture_mutex);
		_rawUpdate = func;
	}
	void setWindowHandle(HWND windowHandle) {
		_windowHandle = windowHandle;
	}
	void copyFrameToTexture(sf::Texture& texture);
	
	bool initCamera();
	void setHDRMode(bool value);
	bool StartCapture();
	bool StopCapture();
	bool isConnected() { return _camera.IsConnected(); }
//	const FlyCapture2::Image& getLastFrame() { return _lastFrame; }
	const FlyCapture2::Image& captureFrame();
	cv::Mat HDRCapture();
	void disconnect();
	~FrameCapture();
};
