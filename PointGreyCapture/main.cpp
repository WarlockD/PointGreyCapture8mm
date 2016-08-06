#include "FrameCapture.h"
#include "logger.h"
#include <opencv2\opencv.hpp>
#include <SFML/Graphics.hpp>
#include <iomanip>

// Library inclusion

#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"jpeg.lib")
#pragma comment(lib,"freetype.lib")


#ifdef _DEBUG
#pragma comment(lib,"FlyCapture2d_v140.lib")
#pragma comment(lib,"opencv_world310d.lib")
#pragma comment(lib,"sfml-main-d.lib")
#pragma comment(lib,"sfml-system-s-d.lib")
#pragma comment(lib,"sfml-window-s-d.lib")
#pragma comment(lib,"sfml-graphics-s-d.lib")

#else
#pragma comment(lib,"FlyCapture2_v140.lib")
#pragma comment(lib,"opencv_world310.lib")
#endif
// constexpr range?
template<typename T> class FixedRange {
	T _min;
	T _max;

};


class RangeConvert {
	const uint32_t _imin;
	const uint32_t _imax;;
	const float _dmin;
	const float _dmax;
public:
	constexpr RangeConvert(uint32_t imin, uint32_t imax, float dmin, float dmax) : _imin(imin), _imax(imax), _dmin(dmin), _dmax(dmax) {}
	constexpr inline double operator()(size_t value) const {
		return ((((float)value - (float)_imin) * (_dmax - _dmin)) / ((float)_imax - (float)_imin)) + _dmin;
	}
};

RangeConvert ExposureConvert(1, 1023, -7.585f, 2.414f);
RangeConvert ShutterConvert(1, 1253, 0.005f, 6.118f);
RangeConvert GainConvert(0, 240, 0.000f, 23.997f);
/** @brief Callback function for a button created by cv::createButton
@param state current state of the button. It could be -1 for a push button, 0 or 1 for a check/radio box button.
@param userdata The optional parameter.
*/
//typedef void(*ButtonCallback)(int state, void* userdata);

void callbackButton(int state, void* userdata) {

}
void callbackButton1(int state, void* userdata) {

}
void callbackButton2(int state, void* userdata) {

}
int value = 50;
int value2 = 0;
cv::String nameb1 = "button1";
cv::String nameb2 = "button2";

void debugMenu() {



	try {
		cv::namedWindow("main1", cv::WINDOW_NORMAL);

		//cv::createTrackbar("track1", "main1", &value, 255, NULL);

		cv::createButton("button1", NULL, NULL, CV_PUSH_BUTTON);

		cv::namedWindow("main2", cv::WINDOW_AUTOSIZE);
		//	cv::createButton(nameb2, &callbackButton, NULL, cv::QT_CHECKBOX, 0);
		//	cv::createTrackbar("track2","main2", &value2, 255, NULL);
		//	cv::createButton("button5", &callbackButton1, NULL, cv::QT_RADIOBOX, 0);
		//	cv::createButton("button6", &callbackButton2, NULL, cv::QT_RADIOBOX, 1);

	}
	catch (cv::Exception e) {
		e.formatMessage();

		std::cerr << e.what() << std::endl;
		//	String msg; ///< the formatted error message

		//	int code; ///< error code @see CVStatus
		//	String err; ///< error description
		//	String func; ///< function name. Available only when the compiler supports getting it
		//	String file; ///< source file name where the error has occured
		//	int line; ///< line number in the source file where the error has occured

		std::cerr << "Cannot create window" << std::endl;

	}




}
typedef union _AbsValueConversion
{
	unsigned long ulValue;
	float fValue;
} AbsValueConversion;

struct exposure_decode {
	uint32_t _raw;
	//bool presence;
//	bool abs_control;
//	bool one_pushed;
//	bool ON_OFF;
//	bool A_M_Mode;
	uint32_t High_value;
	uint32_t Value;
	exposure_decode(int raw) {
		_raw = raw;
		Value = _raw & 0x7FF;
		High_value = (_raw >> 11) & 0xF;
	}
};
void loadExposureSeq(std::vector<cv::Mat>& images, std::vector<float>& times){

}
double timestamp_to_float(int timestamp) {
	uint32_t seconds = (uint32_t)timestamp >> (13 + 12);
	uint32_t cycles = (timestamp >> 12) & 0x1FFF;
	uint32_t parts = (timestamp) & 0xFFF;
	double ret = (float)seconds;
	ret += 0.000125 * cycles;
//	ret += (parts * (1.0 / 0.000125));
	return ret;

}

void tempWindow() {
	// Request a 24-bits depth buffer when creating the window
//	sf::ContextSettings contextSettings;
//	contextSettings.depthBits = 24;

	FrameCapture cap;
	if (!cap.initCamera() || !cap.StartCapture()) return;
	cap.setHDRMode(false);
	// Create the main window
	// Create the window of the application
	sf::RenderWindow window(sf::VideoMode(1024, 768, 32), "Graphics", sf::Style::Titlebar | sf::Style::Close);
	window.setVerticalSyncEnabled(true);

	sf::Clock clock;
	bool isCapturing = false;


	//	debugMenu();
	sf::Texture textureImage;
	textureImage.create(1920, 1200);
	sf::Sprite t_imageWindow;
	sf::Image image;
	//	debugMenu();
	sf::Font font;
	assert(font.loadFromFile("OpenSans-Regular.ttf"));

	sf::Text info;

	info.setFont(font);
	info.setCharacterSize(10);
	info.setPosition(0, 0);
	cap.StartCapture();
	cap.setHDRMode(true);
	int image_number = 0;
	while (window.isOpen())
	{
		// Handle events
		sf::Event event;
		
		while (window.pollEvent(event))
		{
			// Window closed or escape key pressed: exit
			if ((event.type == sf::Event::Closed) ||
				((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)))
			{
				window.close();
				break;
			}
			
			// Space key pressed: play
			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Space))
			{
				switch (event.key.code) {
				case sf::Keyboard::Num1:image_number = 0; break;
				case sf::Keyboard::Num2:image_number = 1; break;
				case sf::Keyboard::Num3:image_number = 2; break;
				case sf::Keyboard::Num4:image_number = 3; break;
				}
			}
		}
		FlyCapture2::Image images[4];
		for (int i = 0; i < 4; i++) {
			images[i] = cap.captureFrame();
		}
		auto& raw_image = images[image_number];
		auto meta = raw_image.GetMetadata();

		FlyCapture2::Image rgbImage; 
		//	raw_image.Convert(FlyCapture2::PIXEL_FORMAT_BGR, &rgbImage);
		raw_image.Convert(FlyCapture2::PIXEL_FORMAT_RGBU, &rgbImage);
	
		std::stringstream ss;
		AbsValueConversion convert; 
		FlyCapture2::TimeStamp timestamp;
	//	if (embeddedInfo.timestamp.available) embeddedInfo.timestamp.onOff = true;
	//	if (embeddedInfo.gain.available) embeddedInfo.gain.onOff = true;
	//	if (embeddedInfo.shutter.available) embeddedInfo.shutter.onOff = true;
	//	if (embeddedInfo.exposure.available) embeddedInfo.exposure.onOff = true;
	//	if (embeddedInfo.brightness.available) embeddedInfo.brightness.onOff = true;
	//	if (embeddedInfo.whiteBalance.available) embeddedInfo.whiteBalance.onOff = true;

		ss << " T:" << std::fixed << std::setprecision(2) << timestamp_to_float(meta.embeddedTimeStamp) << " 0x" << std::hex << meta.embeddedTimeStamp << std::endl;
		double shutter = ShutterConvert(meta.embeddedShutter & 0x7FF);
		ss << " Shutter:"  << std::setprecision(5) << std::fixed << shutter << " 0x" << std::hex << meta.embeddedShutter << std::endl;
		double gain = GainConvert(meta.embeddedGain & 0x7FF);
		ss << " Gain:"  << std::setprecision(5) << std::fixed << gain << " 0x" << std::hex << meta.embeddedGain << std::endl;
		double exposure = ExposureConvert(meta.embeddedExposure & 0x7FF); // 5e-6; // 0.005 ms
		ss << " Exposure:"<< std::setprecision(5) << std::fixed  << exposure << " 0x" << std::hex << meta.embeddedExposure << std::endl;

		info.setString(ss.str());

		// convert to OpenCV Mat
		unsigned int rowBytes = (double)rgbImage.GetReceivedDataSize() / (double)rgbImage.GetRows();
		image.create(rgbImage.GetCols(), rgbImage.GetRows(), rgbImage.GetData());
		cv::Mat mimage = cv::Mat(rgbImage.GetRows(), rgbImage.GetCols(), CV_8UC3, rgbImage.GetData(), rowBytes);
		
		textureImage.update(image);
		t_imageWindow.setTexture(textureImage, true);
		t_imageWindow.setScale(1024.0f/1920.0f, 768.0f / 1200.0f);
		// Clear the window
		window.clear(sf::Color(50, 200, 50));
		window.draw(t_imageWindow);
		window.draw(info);
		window.display();
	}

	cap.StopCapture();
	cap.disconnect();
}
int main(int argc, const char* argv[]) {
	logging::init_cerr();
	logging::init_cout();
	FrameCapture cap;
	tempWindow();
	//debugMenu();
	/*

	if (!cap.initCamera() || !cap.StartCapture()) return -1;

	cap.setHDRMode(false);
	int key = 0;
	while (key != 'q')
	{
		auto& raw_image = cap.captureFrame();
		FlyCapture2::Image rgbImage;
		raw_image.Convert(FlyCapture2::PIXEL_FORMAT_BGR, &rgbImage);
		// convert to OpenCV Mat
		unsigned int rowBytes = (double)rgbImage.GetReceivedDataSize() / (double)rgbImage.GetRows();
		cv::Mat image = cv::Mat(rgbImage.GetRows(), rgbImage.GetCols(), CV_8UC3, rgbImage.GetData(), rowBytes);
		cv::imshow("main2", image);
		key = cv::waitKey(30);

	}

	cap.StopCapture();
	*/
	return 0;
}