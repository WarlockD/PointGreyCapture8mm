#pragma once
#include <FlyCapture2.h>
#include <opencv2\opencv.hpp>

class FrameCapture{
	FlyCapture2::Camera _camera;
	FlyCapture2::CameraInfo _camInfo;
	FlyCapture2::Image _lastFrame;
public:
	FrameCapture();
	bool initCamera();
	void setHDRMode(bool value);
	void DebugTest();
	bool StartCapture();
	bool StopCapture();
	bool isConnected() { return _camera.IsConnected(); }
	const FlyCapture2::Image& getLastFrame() { return _lastFrame; }
	const FlyCapture2::Image& captureFrame();
	cv::Mat HDRCapture();
	void disconnect();
	~FrameCapture();
};

