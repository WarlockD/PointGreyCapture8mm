// stdafx.cpp : source file that includes just the standard includes
//	testwtl.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include <streambuf>
#include <iostream>

#if (_ATL_VER < 0x0700)
#include <atlimpl.cpp>
#endif //(_ATL_VER < 0x0700)
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

// sfml

#pragma comment(lib, "jpeg.lib")
#pragma comment(lib, "freetype.lib")

#pragma comment(lib, "sfml-main.lib")
#pragma comment(lib, "sfml-graphics-d.lib")
#pragma comment(lib, "sfml-system-d.lib")
#pragma comment(lib, "sfml-window-d.lib")

#pragma comment(lib, "FlyCapture2d_v140.lib")
#pragma comment(lib, "opencv_world310d.lib")
namespace
{
	// This class will be used as the default streambuf of sf::Err,
	// it outputs to stderr by default (to keep the default behavior)
	class DefaultErrStreamBuf : public std::streambuf
	{
	public:

		DefaultErrStreamBuf()
		{
			// Allocate the write buffer
			static const int size = 64;
			char* buffer = new char[size];
			setp(buffer, buffer + size);
		}

		~DefaultErrStreamBuf()
		{
			// Synchronize
			sync();

			// Delete the write buffer
			delete[] pbase();
		}
		;
		std::string _stdout;
	private:
		virtual int overflow(int ch)
		{
			if ((ch != traits_type::eof()) && (pptr() != epptr()))
			{
				// Valid character
				if ('\n') {
					sync();
					std::wstring vcout(_stdout.begin(), _stdout.end());
					OutputDebugString(vcout.c_str());
					std::cerr << _stdout << std::endl;
					_stdout.clear();
					return 0; // good
				}
				else sputc(static_cast<char>(ch));
			}
			else if (ch != traits_type::eof())
			{
				// Not enough space in the buffer: synchronize output and try again
				sync();
				return overflow(ch);
			}
			else
			{
				// Invalid character: synchronize output
				return sync();
			}
		}

		virtual int sync()
		{
			// Check if there is something into the write buffer
			if (pbase() != pptr())
			{
				// Print the contents of the write buffer into the standard error output
				std::size_t size = static_cast<int>(pptr() - pbase());
				_stdout.insert(_stdout.end(), pbase(), pptr());
				// Reset the pointer position to the beginning of the write buffer
				setp(pbase(), epptr());
			}

			return 0;
		}
	};
}


	////////////////////////////////////////////////////////////


namespace console {
	static DefaultErrStreamBuf buff;
	void Redirect() {
		AllocConsole();
		std::cout.rdbuf(&buff);
		std::cerr.rdbuf(&buff);
	}
	std::ostream& err()
	{
		static DefaultErrStreamBuf buffer;
		static std::ostream stream(&buffer);
		return stream;
	}
	/// The max length of CCLog message.
	static const int MAX_LOG_LENGTH = 16 * 1024;
	static std::mutex consoleMutex;
	char _logbuffer[MAX_LOG_LENGTH];
	char _logFinalbuffer[MAX_LOG_LENGTH];
	void PrintMessage(const char* type, const char* message) {
		char* msg = _logFinalbuffer; // oldschool booy
		*msg++ = '[';
		while (*type) *msg++ = *type++;
		*msg++ = ']';
		*msg++ = ':';
		*msg++ = ' ';
		while (*message) *msg++ = *message++;
		*msg++ = '\r';
		*msg++ = '\n';
		*msg++ = 0;
		printf(_logFinalbuffer);
#if _DEBUG
		if (OutputDebugStringA == nullptr) {
			//	HINSTANCE dllHandle = LoadLibrary("art.dll");
			//	FindArtistType FindArtistPtr = NULL;

			//Load the dll and keep the handle to it
			//	dllHandle = 
			//	OutputDebugStringA
		}
		OutputDebugStringA(_logFinalbuffer);
#endif

	}
	void info(const char* format, ...) {
		consoleMutex.lock();
		va_list args;
		va_start(args, format);
		vsnprintf_s(_logbuffer, MAX_LOG_LENGTH, format, args);
		va_end(args);
		PrintMessage("INFO", _logbuffer);
		consoleMutex.unlock();
	}
	void error(const char * format, ...) {
		consoleMutex.lock();
		va_list args;
		va_start(args, format);
		vsnprintf_s(_logbuffer, MAX_LOG_LENGTH, format, args);
		va_end(args);
		PrintMessage("ERROR", _logbuffer);
		consoleMutex.unlock();
	}
}