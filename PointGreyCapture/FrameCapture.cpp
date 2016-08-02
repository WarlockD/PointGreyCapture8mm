#include "FrameCapture.h"
#include <opencv2/photo.hpp>
#include <sstream>
#include <iostream>



using namespace FlyCapture2;
using namespace std;

const unsigned int k_HDRCtrl = 0x1800;

const unsigned int k_HDRShutter1 = 0x1820;
const unsigned int k_HDRShutter2 = 0x1840;
const unsigned int k_HDRShutter3 = 0x1860;
const unsigned int k_HDRShutter4 = 0x1880;

const unsigned int k_HDRGain1 = 0x1824;
const unsigned int k_HDRGain2 = 0x1844;
const unsigned int k_HDRGain3 = 0x1864;
const unsigned int k_HDRGain4 = 0x1884;

const unsigned int k_HDROn = 0x82000000;
const unsigned int k_HDROff = 0x80000000;

void PrintError(FlyCapture2::Error error)
{
	error.PrintErrorTrace();
}

void PrintBuildInfo()
{
	FC2Version fc2Version;
	Utilities::GetLibraryVersion(&fc2Version);

	ostringstream version;
	version << "FlyCapture2 library version: " << fc2Version.major << "." << fc2Version.minor << "." << fc2Version.type << "." << fc2Version.build;
	cout << version.str() << endl;

	ostringstream timeStamp;
	timeStamp << "Application build date: " << __DATE__ << " " << __TIME__;
	cout << timeStamp.str() << endl << endl;
}
void PrintCameraInfo(CameraInfo* pCamInfo)
{
	cout << endl;
	cout << "*** CAMERA INFORMATION ***" << endl;
	cout << "Serial number -" << pCamInfo->serialNumber << endl;
	cout << "Camera model - " << pCamInfo->modelName << endl;
	cout << "Camera vendor - " << pCamInfo->vendorName << endl;
	cout << "Sensor - " << pCamInfo->sensorInfo << endl;
	cout << "Resolution - " << pCamInfo->sensorResolution << endl;
	cout << "Firmware version - " << pCamInfo->firmwareVersion << endl;
	cout << "Firmware build time - " << pCamInfo->firmwareBuildTime << endl << endl;

}
bool IsHDRSupported(Camera* pCam)
{
	FlyCapture2::Error error;
	unsigned int RegVal;
	error = pCam->ReadRegister(k_HDRCtrl, &RegVal);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return false;
	}

	return (0x80000000 & RegVal) ? true : false;
}

bool ToggleHDRMode(Camera* pCam, bool hdrOn)
{
	FlyCapture2::Error error;
	error = pCam->WriteRegister(k_HDRCtrl, hdrOn ? k_HDROn : k_HDROff);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return false;
	}

	else cout << "HDR mode turned to " << (hdrOn ? "on" : "off") << endl;
	return true;
}

void InitializeHDRRegisters(Camera* pCam)
{
	FlyCapture2::Error error;
	error = pCam->WriteRegister(k_HDRShutter1, 0x000);
	error = pCam->WriteRegister(k_HDRShutter2, 0x120);
	error = pCam->WriteRegister(k_HDRShutter3, 0x240);
	error = pCam->WriteRegister(k_HDRShutter4, 0x360);

	error = pCam->WriteRegister(k_HDRGain1, 0x000);
	error = pCam->WriteRegister(k_HDRGain2, 0x000);
	error = pCam->WriteRegister(k_HDRGain3, 0x000);
	error = pCam->WriteRegister(k_HDRGain4, 0x000);

	//error = pCam->WriteRegister(k_HDRGain2, 0x0E3);
	//error = pCam->WriteRegister(k_HDRGain3, 0x1C6);
	//error = pCam->WriteRegister(k_HDRGain4, 0x2A9);

	cout << "Initialized HDR registers..." << endl;

	

}



FrameCapture::FrameCapture()
{
}


FrameCapture::~FrameCapture()
{
	disconnect();
}
void FrameCapture::setHDRMode(bool value) {
	InitializeHDRRegisters(&_camera);
	ToggleHDRMode(&_camera, value);
}
const Image& FrameCapture::captureFrame() {
	FlyCapture2::Error error = _camera.RetrieveBuffer(&_lastFrame);
	if (error != PGRERROR_OK) PrintError(error);
	return _lastFrame;
}
bool FrameCapture::StartCapture() {
	FlyCapture2::Error error = _camera.StartCapture();
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return false;
	}
	return true;
}
bool FrameCapture::StopCapture() {
	FlyCapture2::Error error = _camera.StopCapture();
	if (error != PGRERROR_OK)
	{
		// This may fail when the camera was removed, so don't show 
		// an error message
		//PrintError(error);
		return false;
	}
	return true;
}
using namespace cv;

cv::Mat FrameCapture::HDRCapture() {
	Image rawImage[4];
	std::vector<cv::Mat> images;
	std::vector<float> times;
	for (int imageCnt = 0; imageCnt < 4; imageCnt++)
	{
		FlyCapture2::Error error = _camera.RetrieveBuffer(&rawImage[imageCnt]);
		if (error != PGRERROR_OK)
		{
			PrintError(error);
			return cv::Mat();
		}

		cout << "Grabbed image " << imageCnt << endl;
	}
	for (int imageCnt = 0; imageCnt < 4; imageCnt++)
	{
		FlyCapture2::Image rgbImage;
		rawImage[imageCnt].Convert(FlyCapture2::PIXEL_FORMAT_BGR, &rgbImage);

		// convert to OpenCV Mat
		unsigned int rowBytes = (double)rgbImage.GetReceivedDataSize() / (double)rgbImage.GetRows();
		images.push_back(cv::Mat(rgbImage.GetRows(), rgbImage.GetCols(), CV_8UC3, rgbImage.GetData(), rowBytes));
	}
	// Create a converted image
	Image convertedImage;
	// Convert the raw image

	Mat response;
	Ptr<CalibrateDebevec> calibrate = createCalibrateDebevec();
	calibrate->process(images, response, times);

	Mat hdr;
	Ptr<MergeDebevec> merge_debevec = createMergeDebevec();
	merge_debevec->process(images, hdr, times, response);

	Mat ldr;
	Ptr<TonemapDurand> tonemap = createTonemapDurand(2.2f);
	tonemap->process(hdr, ldr);

	Mat fusion;
	Ptr<MergeMertens> merge_mertens = createMergeMertens();
	merge_mertens->process(images, fusion);


	return fusion;
}

void FrameCapture::disconnect() {
	if (isConnected()) {
		// Disconnect the camera
		FlyCapture2::Error error = _camera.Disconnect();
		if (error != PGRERROR_OK) 
			PrintError(error);

	}
}

bool FrameCapture::initCamera() {
	PrintBuildInfo();
	
	FlyCapture2::Error error;

	BusManager busMgr;
	unsigned int numCameras;
	error = busMgr.GetNumOfCameras(&numCameras);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return  false;
	}

	cout << "Number of cameras detected: " << numCameras << endl;

	if (numCameras < 1)
	{
		cout << "Insufficient number of cameras... exiting" << endl;
		return  false;
	}

	PGRGuid guid;
	error = busMgr.GetCameraFromIndex(0, &guid);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return  false;
	}

	// Connect to a camera
	error = _camera.Connect(&guid);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return  false;
	}
	// Get the camera information
	error = _camera.GetCameraInfo(&_camInfo);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return  false;
	}
	PrintCameraInfo(&_camInfo);

	// Enable timestamping
	EmbeddedImageInfo embeddedInfo;

	error = _camera.GetEmbeddedImageInfo(&embeddedInfo);
	if (error != PGRERROR_OK) PrintError(error);


	if (embeddedInfo.timestamp.available) embeddedInfo.timestamp.onOff = true;
	if (embeddedInfo.gain.available) embeddedInfo.gain.onOff = true;
	if (embeddedInfo.shutter.available) embeddedInfo.shutter.onOff = true;
	if (embeddedInfo.exposure.available) embeddedInfo.exposure.onOff = true;
	if (embeddedInfo.brightness.available) embeddedInfo.brightness.onOff = true;
	if (embeddedInfo.whiteBalance.available) embeddedInfo.whiteBalance.onOff = true;

	error = _camera.SetEmbeddedImageInfo(&embeddedInfo);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return false;
	}


	return true;
}