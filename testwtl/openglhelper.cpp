#pragma once
#include "stdafx.h"

#undef min
#undef max
#include <cassert>

#include <cmath>
#include <algorithm>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define GL_CLAMP_TO_EDGE 0x812F
#define TJE_IMPLEMENTATION
#include "tiny_jpeg.h"
#include "GLLoader.h"

// gl constants

// SFML requires at a bare minimum OpenGL 1.1 capability
// All functionality beyond that is optional
// and has to be checked for prior to use


////////////////////////////////////////////////////////////
void glCheckError(const char* file, unsigned int line, const char* expression)
{
	// Get the last error
	GLenum errorCode = glGetError();

	if (errorCode != GL_NO_ERROR)
	{
		std::string fileString = file;
		std::string error = "Unknown error";
		std::string description = "No description";

		// Decode the error code
		switch (errorCode)
		{
		case GL_INVALID_ENUM:
		{
			error = "GL_INVALID_ENUM";
			description = "An unacceptable value has been specified for an enumerated argument.";
			break;
		}

		case GL_INVALID_VALUE:
		{
			error = "GL_INVALID_VALUE";
			description = "A numeric argument is out of range.";
			break;
		}

		case GL_INVALID_OPERATION:
		{
			error = "GL_INVALID_OPERATION";
			description = "The specified operation is not allowed in the current state.";
			break;
		}

		case GL_STACK_OVERFLOW:
		{
			error = "GL_STACK_OVERFLOW";
			description = "This command would cause a stack overflow.";
			break;
		}

		case GL_STACK_UNDERFLOW:
		{
			error = "GL_STACK_UNDERFLOW";
			description = "This command would cause a stack underflow.";
			break;
		}

		case GL_OUT_OF_MEMORY:
		{
			error = "GL_OUT_OF_MEMORY";
			description = "There is not enough memory left to execute the command.";
			break;
		}

		//	case GLEXT_GL_INVALID_FRAMEBUFFER_OPERATION:
		//	{
		////		error = "GL_INVALID_FRAMEBUFFER_OPERATION";
		//		description = "The object bound to FRAMEBUFFER_BINDING is not \"framebuffer complete\".";
		//		break;
		//	}
		}
		// Log the error
		err() << "An internal OpenGL call failed in "
			<< fileString.substr(fileString.find_last_of("\\/") + 1) << "(" << line << ")."
			<< "\nExpression:\n   " << expression
			<< "\nError description:\n   " << error << "\n   " << description << "\n"
			<< std::endl;
	}
}


namespace
{
	// Thread-safe unique identifier generator,
	// is used for states cache (see RenderTarget)
	uint64_t getUniqueId()
	{
		static std::atomic<uint64_t> id = 1;
		return id++;
	}

	unsigned int checkMaximumTextureSize()
	{
		GLint size;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size);
		return static_cast<unsigned int>(size);
	}
	// stb_image callbacks that operate on a sf::InputStream
	int stbi_io_callback_read(void* user, char* data, int size)
	{
		std::istream* stream = static_cast<std::istream*>(user);
		stream->read(data, size);
		return static_cast<int>(stream->gcount());
	}
	void stbi_io_callback_skip(void* user, int size)
	{
		std::istream* stream = static_cast<std::istream*>(user);
		stream->seekg(size, std::ios::cur);
	}
	int stbi_io_callback_eof(void* user)
	{
		std::istream* stream = static_cast<std::istream*>(user);
		return stream->eof();
	}
	// Convert an sf::BlendMode::Factor constant to the corresponding OpenGL constant.
	uint32_t factorToGlConstant(sf::BlendMode::Factor blendFactor)
	{
		switch (blendFactor)
		{
		case sf::BlendMode::Zero:             return GL_ZERO;
		case sf::BlendMode::One:              return GL_ONE;
		case sf::BlendMode::SrcColor:         return GL_SRC_COLOR;
		case sf::BlendMode::OneMinusSrcColor: return GL_ONE_MINUS_SRC_COLOR;
		case sf::BlendMode::DstColor:         return GL_DST_COLOR;
		case sf::BlendMode::OneMinusDstColor: return GL_ONE_MINUS_DST_COLOR;
		case sf::BlendMode::SrcAlpha:         return GL_SRC_ALPHA;
		case sf::BlendMode::OneMinusSrcAlpha: return GL_ONE_MINUS_SRC_ALPHA;
		case sf::BlendMode::DstAlpha:         return GL_DST_ALPHA;
		case sf::BlendMode::OneMinusDstAlpha: return GL_ONE_MINUS_DST_ALPHA;
		}

		err() << "Invalid value for sf::BlendMode::Factor! Fallback to sf::BlendMode::Zero." << std::endl;
		assert(false);
		return GL_ZERO;
	}


	// Convert an sf::BlendMode::BlendEquation constant to the corresponding OpenGL constant.
	uint32_t equationToGlConstant(sf::BlendMode::Equation blendEquation)
	{
		switch (blendEquation)
		{
		case sf::BlendMode::Add:             return GLEXT_GL_FUNC_ADD;
		case sf::BlendMode::Subtract:        return GLEXT_GL_FUNC_SUBTRACT;
		}

		err() << "Invalid value for sf::BlendMode::Equation! Fallback to sf::BlendMode::Add." << std::endl;
		assert(false);
		return GLEXT_GL_FUNC_ADD;
	}
	////////////////////////////////////////////////////////////
	void ensureExtensionsInit(HDC deviceContext)
	{
		static bool initialized = false;
		if (!initialized)
		{
			initialized = true;

			// We don't check the return value since the extension
			// flags are cleared even if loading fails
			sfwgl_LoadFunctions(deviceContext);
		}
	}


	////////////////////////////////////////////////////////////
	std::wstring getErrorString(DWORD errorCode)
	{
		std::wstringstream ss;
		TCHAR errBuff[256];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorCode, 0, errBuff, sizeof(errBuff), NULL);
		ss << errBuff;
		return ss.str();
	}
}
namespace priv {
	class TextureSaver {
		GLint m_textureBinding; ///< Texture binding to restore
	public:
		TextureSaver() { glCheck(glGetIntegerv(GL_TEXTURE_BINDING_2D, &m_textureBinding)); }
		~TextureSaver() { glCheck(glBindTexture(GL_TEXTURE_2D, m_textureBinding)); }

	};
	void ensureExtensionsInit()
	{
#if !defined(SFML_OPENGL_ES)
		static bool initialized = false;
		if (!initialized)
		{
			sfogl_LoadFunctions();

			if (!sfogl_IsVersionGEQ(1, 1))
			{
				err() << "sfml-graphics requires support for OpenGL 1.1 or greater" << std::endl;
				err() << "Ensure that hardware acceleration is enabled if available" << std::endl;
			}

			initialized = true;
		}
#endif
	}

};

namespace sf {
	const Transform Transform::Identity;


	////////////////////////////////////////////////////////////
	Transform::Transform()
	{
		// Identity matrix
		m_matrix[0] = 1.f; m_matrix[4] = 0.f; m_matrix[8] = 0.f; m_matrix[12] = 0.f;
		m_matrix[1] = 0.f; m_matrix[5] = 1.f; m_matrix[9] = 0.f; m_matrix[13] = 0.f;
		m_matrix[2] = 0.f; m_matrix[6] = 0.f; m_matrix[10] = 1.f; m_matrix[14] = 0.f;
		m_matrix[3] = 0.f; m_matrix[7] = 0.f; m_matrix[11] = 0.f; m_matrix[15] = 1.f;
	}


	////////////////////////////////////////////////////////////
	Transform::Transform(float a00, float a01, float a02,
		float a10, float a11, float a12,
		float a20, float a21, float a22)
	{
		m_matrix[0] = a00; m_matrix[4] = a01; m_matrix[8] = 0.f; m_matrix[12] = a02;
		m_matrix[1] = a10; m_matrix[5] = a11; m_matrix[9] = 0.f; m_matrix[13] = a12;
		m_matrix[2] = 0.f; m_matrix[6] = 0.f; m_matrix[10] = 1.f; m_matrix[14] = 0.f;
		m_matrix[3] = a20; m_matrix[7] = a21; m_matrix[11] = 0.f; m_matrix[15] = a22;
	}



	////////////////////////////////////////////////////////////
	Transform Transform::getInverse() const
	{
		// Compute the determinant
		float det = m_matrix[0] * (m_matrix[15] * m_matrix[5] - m_matrix[7] * m_matrix[13]) -
			m_matrix[1] * (m_matrix[15] * m_matrix[4] - m_matrix[7] * m_matrix[12]) +
			m_matrix[3] * (m_matrix[13] * m_matrix[4] - m_matrix[5] * m_matrix[12]);

		// Compute the inverse if the determinant is not zero
		// (don't use an epsilon because the determinant may *really* be tiny)
		if (det != 0.f)
		{
			return Transform((m_matrix[15] * m_matrix[5] - m_matrix[7] * m_matrix[13]) / det,
				-(m_matrix[15] * m_matrix[4] - m_matrix[7] * m_matrix[12]) / det,
				(m_matrix[13] * m_matrix[4] - m_matrix[5] * m_matrix[12]) / det,
				-(m_matrix[15] * m_matrix[1] - m_matrix[3] * m_matrix[13]) / det,
				(m_matrix[15] * m_matrix[0] - m_matrix[3] * m_matrix[12]) / det,
				-(m_matrix[13] * m_matrix[0] - m_matrix[1] * m_matrix[12]) / det,
				(m_matrix[7] * m_matrix[1] - m_matrix[3] * m_matrix[5]) / det,
				-(m_matrix[7] * m_matrix[0] - m_matrix[3] * m_matrix[4]) / det,
				(m_matrix[5] * m_matrix[0] - m_matrix[1] * m_matrix[4]) / det);
		}
		else
		{
			return Identity;
		}
	}

	Vector2f Transform::transformPoint(float x, float y) const
	{
		return Vector2f(m_matrix[0] * x + m_matrix[4] * y + m_matrix[12],
			m_matrix[1] * x + m_matrix[5] * y + m_matrix[13]);
	}

	FloatRect Transform::transformRect(const FloatRect& rectangle) const
	{
		// Transform the 4 corners of the rectangle
		const Vector2f points[] =
		{
			transformPoint(rectangle.left, rectangle.top),
			transformPoint(rectangle.left, rectangle.top + rectangle.height),
			transformPoint(rectangle.left + rectangle.width, rectangle.top),
			transformPoint(rectangle.left + rectangle.width, rectangle.top + rectangle.height)
		};

		// Compute the bounding rectangle of the transformed points
		float left = points[0].x;
		float top = points[0].y;
		float right = points[0].x;
		float bottom = points[0].y;
		for (int i = 1; i < 4; ++i)
		{
			if (points[i].x < left)   left = points[i].x;
			else if (points[i].x > right)  right = points[i].x;
			if (points[i].y < top)    top = points[i].y;
			else if (points[i].y > bottom) bottom = points[i].y;
		}

		return FloatRect(left, top, right - left, bottom - top);
	}

	Transform& Transform::combine(const Transform& transform)
	{
		const float* a = m_matrix;
		const float* b = transform.m_matrix;

		*this = Transform(a[0] * b[0] + a[4] * b[1] + a[12] * b[3],
			a[0] * b[4] + a[4] * b[5] + a[12] * b[7],
			a[0] * b[12] + a[4] * b[13] + a[12] * b[15],
			a[1] * b[0] + a[5] * b[1] + a[13] * b[3],
			a[1] * b[4] + a[5] * b[5] + a[13] * b[7],
			a[1] * b[12] + a[5] * b[13] + a[13] * b[15],
			a[3] * b[0] + a[7] * b[1] + a[15] * b[3],
			a[3] * b[4] + a[7] * b[5] + a[15] * b[7],
			a[3] * b[12] + a[7] * b[13] + a[15] * b[15]);

		return *this;
	}


	Transform& Transform::translate(float x, float y)
	{
		Transform translation(1, 0, x,
			0, 1, y,
			0, 0, 1);

		return combine(translation);
	}


	Transform& Transform::rotate(float angle)
	{
		float rad = angle * 3.141592654f / 180.f;
		float cos = std::cos(rad);
		float sin = std::sin(rad);

		Transform rotation(cos, -sin, 0,
			sin, cos, 0,
			0, 0, 1);

		return combine(rotation);
	}


	Transform& Transform::rotate(float angle, float centerX, float centerY)
	{
		float rad = angle * 3.141592654f / 180.f;
		float cos = std::cos(rad);
		float sin = std::sin(rad);

		Transform rotation(cos, -sin, centerX * (1 - cos) + centerY * sin,
			sin, cos, centerY * (1 - cos) - centerX * sin,
			0, 0, 1);

		return combine(rotation);
	}

	Transform& Transform::scale(float scaleX, float scaleY)
	{
		Transform scaling(scaleX, 0, 0,
			0, scaleY, 0,
			0, 0, 1);

		return combine(scaling);
	}

	Transform& Transform::scale(float scaleX, float scaleY, float centerX, float centerY)
	{
		Transform scaling(scaleX, 0, centerX * (1 - scaleX),
			0, scaleY, centerY * (1 - scaleY),
			0, 0, 1);

		return combine(scaling);
	}

	////////////////////////////////////////////////////////////
	// Commonly used blending modes
	////////////////////////////////////////////////////////////
	const BlendMode BlendAlpha(BlendMode::SrcAlpha, BlendMode::OneMinusSrcAlpha, BlendMode::Add,
		BlendMode::One, BlendMode::OneMinusSrcAlpha, BlendMode::Add);
	const BlendMode BlendAdd(BlendMode::SrcAlpha, BlendMode::One, BlendMode::Add,
		BlendMode::One, BlendMode::One, BlendMode::Add);
	const BlendMode BlendMultiply(BlendMode::DstColor, BlendMode::Zero);
	const BlendMode BlendNone(BlendMode::One, BlendMode::Zero);

	////////////////////////////////////////////////////////////
	BlendMode::BlendMode() :
		colorSrcFactor(BlendMode::SrcAlpha),
		colorDstFactor(BlendMode::OneMinusSrcAlpha),
		colorEquation(BlendMode::Add),
		alphaSrcFactor(BlendMode::One),
		alphaDstFactor(BlendMode::OneMinusSrcAlpha),
		alphaEquation(BlendMode::Add)
	{

	}


	////////////////////////////////////////////////////////////
	BlendMode::BlendMode(Factor sourceFactor, Factor destinationFactor, Equation blendEquation) :
		colorSrcFactor(sourceFactor),
		colorDstFactor(destinationFactor),
		colorEquation(blendEquation),
		alphaSrcFactor(sourceFactor),
		alphaDstFactor(destinationFactor),
		alphaEquation(blendEquation)
	{

	}


	////////////////////////////////////////////////////////////
	BlendMode::BlendMode(Factor colorSourceFactor, Factor colorDestinationFactor,
		Equation colorBlendEquation, Factor alphaSourceFactor,
		Factor alphaDestinationFactor, Equation alphaBlendEquation) :
		colorSrcFactor(colorSourceFactor),
		colorDstFactor(colorDestinationFactor),
		colorEquation(colorBlendEquation),
		alphaSrcFactor(alphaSourceFactor),
		alphaDstFactor(alphaDestinationFactor),
		alphaEquation(alphaBlendEquation)
	{

	}


	////////////////////////////////////////////////////////////
	bool operator ==(const BlendMode& left, const BlendMode& right) 
	{
		return (left.colorSrcFactor == right.colorSrcFactor) &&
			(left.colorDstFactor == right.colorDstFactor) &&
			(left.colorEquation == right.colorEquation) &&
			(left.alphaSrcFactor == right.alphaSrcFactor) &&
			(left.alphaDstFactor == right.alphaDstFactor) &&
			(left.alphaEquation == right.alphaEquation);
	}


	////////////////////////////////////////////////////////////
	bool operator !=(const BlendMode& left, const BlendMode& right) 
	{
		return !(left == right);
	}
	const Color Color::Black(0, 0, 0);
	const Color Color::White(255, 255, 255);
	const Color Color::Red(255, 0, 0);
	const Color Color::Green(0, 255, 0);
	const Color Color::Blue(0, 0, 255);
	const Color Color::Yellow(255, 255, 0);
	const Color Color::Magenta(255, 0, 255);
	const Color Color::Cyan(0, 255, 255);
	const Color Color::Transparent(0, 0, 0, 0);



	////////////////////////////////////////////////////////////
	Color operator +(const Color& left, const Color& right)
	{
		return Color(uint8_t(std::min(int(left.r) + right.r, 255)),
			uint8_t(std::min(int(left.g) + right.g, 255)),
			uint8_t(std::min(int(left.b) + right.b, 255)),
			uint8_t(std::min(int(left.a) + right.a, 255)));
	}


	////////////////////////////////////////////////////////////
	Color operator -(const Color& left, const Color& right)
	{
		return Color(uint8_t(std::max(int(left.r) - right.r, 0)),
			uint8_t(std::max(int(left.g) - right.g, 0)),
			uint8_t(std::max(int(left.b) - right.b, 0)),
			uint8_t(std::max(int(left.a) - right.a, 0)));
	}


	////////////////////////////////////////////////////////////
	Color operator *(const Color& left, const Color& right)
	{
		return Color(uint8_t(int(left.r) * right.r / 255),
			uint8_t(int(left.g) * right.g / 255),
			uint8_t(int(left.b) * right.b / 255),
			uint8_t(int(left.a) * right.a / 255));
	}
	// We cannot use the default constructor here, because it accesses BlendAlpha, which is also global (and dynamically
	// initialized). Initialization order of global objects in different translation units is not defined.
	const RenderStates RenderStates::Default(BlendMode(
		BlendMode::SrcAlpha, BlendMode::OneMinusSrcAlpha, BlendMode::Add,
		BlendMode::One, BlendMode::OneMinusSrcAlpha, BlendMode::Add));

	// Image

	void Image::create(unsigned int width, unsigned int height, const Color& color)
	{
		_pixels.clear();
		if (width && height)
		{
			_pixels.resize(width, height * 4);
			_size = Vector2u(width, height);
			uint32_t* pixels = reinterpret_cast<uint32_t*>(_pixels.data());
			std::fill(pixels, pixels + _pixels.size()/ 4, color.toInteger());
		}
		else _size = Vector2u(0, 0);
	}
	void Image::create(unsigned int width, unsigned int height, std::vector<uint8_t>&& move) {
		_pixels.clear();
		if (width && height)
		{
			_pixels = std::move(move);
			_size = Vector2u(width, height);
		}
		else _size = Vector2u(0, 0);
	}

	////////////////////////////////////////////////////////////
	void Image::create(unsigned int width, unsigned int height, const uint8_t* pixels)
	{
		_pixels.clear();
		if (width && height)
		{
			_size = Vector2u(width, height);
			_pixels.assign(pixels, pixels + (width*height * 4));
		}
		else _size = Vector2u(0, 0);
	}


	////////////////////////////////////////////////////////////
	bool Image::loadFromFile(const std::string& filename)
	{
		// Load the image and get a pointer to the pixels in memory
		int width, height, channels;
		unsigned char* ptr = stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb_alpha);
		if (ptr && width && height)
		{
			create(width, height, ptr);
			stbi_image_free(ptr);
			return true;
		}
		// Error, failed to load the image
		err() << "Failed to load image \"" << filename << "\". Reason: " << stbi_failure_reason() << std::endl;
		return false;
	}


	////////////////////////////////////////////////////////////
	bool Image::loadFromMemory(const void* data, std::size_t dataSize)
	{
		if (data && dataSize)
		{
			// Load the image and get a pointer to the pixels in memory
			int width, height, channels;
			unsigned char* ptr = stbi_load_from_memory(static_cast<const unsigned char*>(data), static_cast<int>(dataSize), &width, &height, &channels, STBI_rgb_alpha);
			if (ptr && width && height)
			{
				create(width, height, ptr);
				stbi_image_free(ptr);
				return true;
			}
			err() << "Failed to load image from memory. Reason: " << stbi_failure_reason() << std::endl;
			return false;
		} 
		err() << "Failed to load image from memory, no data provided" << std::endl;
		return false;
	}


	////////////////////////////////////////////////////////////
	bool Image::loadFromStream(std::istream& stream)
	{
		// Setup the stb_image callbacks
		stbi_io_callbacks callbacks;
		callbacks.read = &stbi_io_callback_read;
		callbacks.skip = &stbi_io_callback_skip;
		callbacks.eof = &stbi_io_callback_eof;
		// Load the image and get a pointer to the pixels in memory
		int width, height, channels;
		unsigned char* ptr = stbi_load_from_callbacks(&callbacks, &stream, &width, &height, &channels, STBI_rgb_alpha);
		if (ptr && width && height)
		{
			create(width, height, ptr);
			stbi_image_free(ptr);
			return true;
		}
		// Error, failed to load the image
		err() << "Failed to load image from stream. Reason: " << stbi_failure_reason() << std::endl;
		return false;
	}


	////////////////////////////////////////////////////////////
	bool Image::saveToFile(const std::string& filename) const
	{
		if (_pixels.size() > 0) {
			// Deduce the image type from its extension

			// Extract the extension
			const std::size_t dot = filename.find_last_of('.');
			if (dot != std::string::npos) {
				std::string extension;
				std::transform(filename.begin() + dot, filename.end(), extension.begin(), ::tolower);
				if (extension == "bmp")
				{
					// BMP format
					if (stbi_write_bmp(filename.c_str(), _size.x, _size.y, 4, _pixels.data()))
						return true;
				}
				else if (extension == "tga")
				{
					// TGA format
					if (stbi_write_tga(filename.c_str(), _size.x, _size.y, 4, _pixels.data()))
						return true;
				}
				else if (extension == "png")
				{
					// PNG format
					if (stbi_write_png(filename.c_str(), _size.x, _size.y, 4, _pixels.data(), 0))
						return true;
				}
				else if (extension == "jpg" || extension == "jpeg") {
					if (tje_encode_to_file(filename.c_str(), _size.x, _size.y, 4, _pixels.data()))
						return true;

				}
			}
			 // no jpeg support:(

		}
		err() << "Failed to save image \"" << filename << "\"" << std::endl;
		return false;
	}


	////////////////////////////////////////////////////////////
	void Image::createMaskFromColor(const Color& color, uint8_t alpha)
	{
		// Make sure that the image is not empty
		if (_pixels.size() > 0)
		{
			// Replace the alpha of the pixels that match the transparent color
			uint8_t* ptr = _pixels.data();
			uint8_t* end = ptr + _pixels.size();
			while (ptr < end)
			{
				if ((ptr[0] == color.r) && (ptr[1] == color.g) && (ptr[2] == color.b) && (ptr[3] == color.a))
					ptr[3] = alpha;
				ptr += 4;
			}
		}
	}


	////////////////////////////////////////////////////////////
	void Image::copy(const Image& source, unsigned int destX, unsigned int destY, const IntRect& sourceRect, bool applyAlpha)
	{
		// Make sure that both images are valid
		if ((source._size.x == 0) || (source._size.y == 0) || (_size.x == 0) || (_size.y == 0))
			return;

		// Adjust the source rectangle
		IntRect srcRect = sourceRect;
		if (srcRect.width == 0 || (srcRect.height == 0))
		{
			srcRect.left = 0;
			srcRect.top = 0;
			srcRect.width = source._size.x;
			srcRect.height = source._size.y;
		}
		else
		{
			if (srcRect.left   < 0) srcRect.left = 0;
			if (srcRect.top    < 0) srcRect.top = 0;
			if (srcRect.width  > static_cast<int>(source._size.x)) srcRect.width = source._size.x;
			if (srcRect.height > static_cast<int>(source._size.y)) srcRect.height = source._size.y;
		}

		// Then find the valid bounds of the destination rectangle
		int width = srcRect.width;
		int height = srcRect.height;
		if (destX + width  > _size.x) width = _size.x - destX;
		if (destY + height > _size.y) height = _size.y - destY;

		// Make sure the destination area is valid
		if ((width <= 0) || (height <= 0))
			return;

		// Precompute as much as possible
		const int          pitch = width * 4;
		const int          rows = height;
		const int          srcStride = source._size.x * 4;
		const  int          dstStride = _size.x * 4;
		const uint8_t* srcPixels = &source._pixels[0] + (srcRect.left + srcRect.top * source._size.x) * 4;
		uint8_t*       dstPixels = &_pixels[0] + (destX + destY * _size.x) * 4;

		// Copy the pixels
		if (applyAlpha)
		{
			// Interpolation using alpha values, pixel by pixel (slower)
			for (int i = 0; i < rows; ++i)
			{
				for (int j = 0; j < width; ++j)
				{
					// Get a direct pointer to the components of the current pixel
					const uint8_t* src = srcPixels + j * 4;
					uint8_t*       dst = dstPixels + j * 4;

					// Interpolate RGBA components using the alpha value of the source pixel
					uint8_t alpha = src[3];
					dst[0] = (src[0] * alpha + dst[0] * (255 - alpha)) / 255;
					dst[1] = (src[1] * alpha + dst[1] * (255 - alpha)) / 255;
					dst[2] = (src[2] * alpha + dst[2] * (255 - alpha)) / 255;
					dst[3] = alpha + dst[3] * (255 - alpha) / 255;
				}

				srcPixels += srcStride;
				dstPixels += dstStride;
			}
		}
		else
		{
			// Optimized copy ignoring alpha values, row by row (faster)
			for (int i = 0; i < rows; ++i)
			{
				std::memcpy(dstPixels, srcPixels, pitch);
				srcPixels += srcStride;
				dstPixels += dstStride;
			}
		}
	}


	////////////////////////////////////////////////////////////
	void Image::setPixel(unsigned int x, unsigned int y, const Color& color)
	{
		uint8_t* pixel = &_pixels[(x + y * _size.x) * 4];
		*pixel++ = color.r;
		*pixel++ = color.g;
		*pixel++ = color.b;
		*pixel++ = color.a;
	}


	////////////////////////////////////////////////////////////
	Color Image::getPixel(unsigned int x, unsigned int y) const
	{
		const uint8_t* pixel = &_pixels[(x + y * _size.x) * 4];
		return Color(pixel[0], pixel[1], pixel[2], pixel[3]);
	}



	////////////////////////////////////////////////////////////
	void Image::flipHorizontally()
	{
		if (!_pixels.empty())
		{
			std::size_t rowSize = _size.x * 4;

			for (std::size_t y = 0; y < _size.y; ++y)
			{
				std::vector<uint8_t>::iterator left = _pixels.begin() + y * rowSize;
				std::vector<uint8_t>::iterator right = _pixels.begin() + (y + 1) * rowSize - 4;

				for (std::size_t x = 0; x < _size.x / 2; ++x)
				{
					std::swap_ranges(left, left + 4, right);

					left += 4;
					right -= 4;
				}
			}
		}
	}


	////////////////////////////////////////////////////////////
	void Image::flipVertically()
	{
		if (!_pixels.empty())
		{
			std::size_t rowSize = _size.x * 4;

			std::vector<uint8_t>::iterator top = _pixels.begin();
			std::vector<uint8_t>::iterator bottom = _pixels.end() - rowSize;

			for (std::size_t y = 0; y < _size.y / 2; ++y)
			{
				std::swap_ranges(top, top + rowSize, bottom);

				top += rowSize;
				bottom -= rowSize;
			}
		}
	}


	View::View() :
		m_center(),
		m_size(),
		m_rotation(0),
		m_viewport(0, 0, 1, 1),
		m_transformUpdated(false),
		m_invTransformUpdated(false)
	{
		reset(FloatRect(0, 0, 1000, 1000));
	}


	////////////////////////////////////////////////////////////
	View::View(const FloatRect& rectangle) :
		m_center(),
		m_size(),
		m_rotation(0),
		m_viewport(0, 0, 1, 1),
		m_transformUpdated(false),
		m_invTransformUpdated(false)
	{
		reset(rectangle);
	}


	////////////////////////////////////////////////////////////
	View::View(const Vector2f& center, const Vector2f& size) :
		m_center(center),
		m_size(size),
		m_rotation(0),
		m_viewport(0, 0, 1, 1),
		m_transformUpdated(false),
		m_invTransformUpdated(false)
	{

	}

	////////////////////////////////////////////////////////////
	void View::setCenter(float x, float y)
	{
		m_center.x = x;
		m_center.y = y;

		m_transformUpdated = false;
		m_invTransformUpdated = false;
	}


	////////////////////////////////////////////////////////////
	void View::setCenter(const Vector2f& center)
	{
		setCenter(center.x, center.y);
	}


	////////////////////////////////////////////////////////////
	void View::setSize(float width, float height)
	{
		m_size.x = width;
		m_size.y = height;

		m_transformUpdated = false;
		m_invTransformUpdated = false;
	}


	////////////////////////////////////////////////////////////
	void View::setSize(const Vector2f& size)
	{
		setSize(size.x, size.y);
	}


	////////////////////////////////////////////////////////////
	void View::setRotation(float angle)
	{
		m_rotation = static_cast<float>(fmod(angle, 360));
		if (m_rotation < 0)
			m_rotation += 360.f;

		m_transformUpdated = false;
		m_invTransformUpdated = false;
	}


	////////////////////////////////////////////////////////////
	void View::setViewport(const FloatRect& viewport)
	{
		m_viewport = viewport;
	}


	////////////////////////////////////////////////////////////
	void View::reset(const FloatRect& rectangle)
	{
		m_center.x = rectangle.left + rectangle.width / 2.f;
		m_center.y = rectangle.top + rectangle.height / 2.f;
		m_size.x = rectangle.width;
		m_size.y = rectangle.height;
		m_rotation = 0;

		m_transformUpdated = false;
		m_invTransformUpdated = false;
	}


	////////////////////////////////////////////////////////////
	const Vector2f& View::getCenter() const
	{
		return m_center;
	}


	////////////////////////////////////////////////////////////
	const Vector2f& View::getSize() const
	{
		return m_size;
	}


	////////////////////////////////////////////////////////////
	float View::getRotation() const
	{
		return m_rotation;
	}


	////////////////////////////////////////////////////////////
	const FloatRect& View::getViewport() const
	{
		return m_viewport;
	}


	////////////////////////////////////////////////////////////
	void View::move(float offsetX, float offsetY)
	{
		setCenter(m_center.x + offsetX, m_center.y + offsetY);
	}


	////////////////////////////////////////////////////////////
	void View::move(const Vector2f& offset)
	{
		setCenter(m_center + offset);
	}


	////////////////////////////////////////////////////////////
	void View::rotate(float angle)
	{
		setRotation(m_rotation + angle);
	}


	////////////////////////////////////////////////////////////
	void View::zoom(float factor)
	{
		setSize(m_size.x * factor, m_size.y * factor);
	}


	////////////////////////////////////////////////////////////
	const Transform& View::getTransform() const
	{
		// Recompute the matrix if needed
		if (!m_transformUpdated)
		{
			// Rotation components
			float angle = m_rotation * 3.141592654f / 180.f;
			float cosine = static_cast<float>(std::cos(angle));
			float sine = static_cast<float>(std::sin(angle));
			float tx = -m_center.x * cosine - m_center.y * sine + m_center.x;
			float ty = m_center.x * sine - m_center.y * cosine + m_center.y;

			// Projection components
			float a = 2.f / m_size.x;
			float b = -2.f / m_size.y;
			float c = -a * m_center.x;
			float d = -b * m_center.y;

			// Rebuild the projection matrix
			m_transform = Transform(a * cosine, a * sine, a * tx + c,
				-b * sine, b * cosine, b * ty + d,
				0.f, 0.f, 1.f);
			m_transformUpdated = true;
		}

		return m_transform;
	}


	////////////////////////////////////////////////////////////
	const Transform& View::getInverseTransform() const
	{
		// Recompute the matrix if needed
		if (!m_invTransformUpdated)
		{
			m_inverseTransform = getTransform().getInverse();
			m_invTransformUpdated = true;
		}

		return m_inverseTransform;
	}

	////////////////////////////////////////////////////////////
	Texture::Texture() :
		m_size(0, 0),
		m_actualSize(0, 0),
		m_texture(0),
		m_isSmooth(false),
		m_isRepeated(false),
		m_pixelsFlipped(false),
		m_fboAttachment(false),
		m_cacheId(getUniqueId())
	{
	}


	////////////////////////////////////////////////////////////
	Texture::Texture(const Texture& copy) :
		m_size(0, 0),
		m_actualSize(0, 0),
		m_texture(0),
		m_isSmooth(copy.m_isSmooth),
		m_isRepeated(copy.m_isRepeated),
		m_pixelsFlipped(false),
		m_fboAttachment(false),
		m_cacheId(getUniqueId())
	{
		if (copy.m_texture)
			loadFromImage(copy.copyToImage());
	}


	////////////////////////////////////////////////////////////
	Texture::~Texture()
	{
		// Destroy the OpenGL texture
		if (m_texture)
		{
#if 0
			ensureGlContext();
#endif
			GLuint texture = static_cast<GLuint>(m_texture);
			glCheck(glDeleteTextures(1, &texture));
		}
	}


	////////////////////////////////////////////////////////////
	bool Texture::create(unsigned int width, unsigned int height)
	{
		// Check if texture parameters are valid before creating it
		if ((width == 0) || (height == 0))
		{
			err() << "Failed to create texture, invalid size (" << width << "x" << height << ")" << std::endl;
			return false;
		}

		// Compute the internal texture dimensions depending on NPOT textures support
		Vector2u actualSize(getValidSize(width), getValidSize(height));

		// Check the maximum texture size
		unsigned int maxSize = getMaximumSize();
		if ((actualSize.x > maxSize) || (actualSize.y > maxSize))
		{
			err() << "Failed to create texture, its internal size is too high "
				<< "(" << actualSize.x << "x" << actualSize.y << ", "
				<< "maximum is " << maxSize << "x" << maxSize << ")"
				<< std::endl;
			return false;
		}

		// All the validity checks passed, we can store the new texture settings
		m_size.x = width;
		m_size.y = height;
		m_actualSize = actualSize;
		m_pixelsFlipped = false;
		m_fboAttachment = false;
#if 0
		ensureGlContext();
#endif
		// Create the OpenGL texture if it doesn't exist yet
		if (!m_texture)
		{
			GLuint texture;
			glCheck(glGenTextures(1, &texture));
			m_texture = static_cast<unsigned int>(texture);
		}
#if 0
		// Make sure that extensions are initialized
		priv::ensureExtensionsInit();

		// Make sure that the current texture binding will be preserved
		priv::TextureSaver save;

		static bool textureEdgeClamp = GLEXT_texture_edge_clamp || GLEXT_EXT_texture_edge_clamp;

		if (!m_isRepeated && !textureEdgeClamp)
		{
			static bool warned = false;

			if (!warned)
			{
				err() << "OpenGL extension SGIS_texture_edge_clamp unavailable" << std::endl;
				err() << "Artifacts may occur along texture edges" << std::endl;
				err() << "Ensure that hardware acceleration is enabled if available" << std::endl;

				warned = true;
			}
		}

		// Initialize the texture
		glCheck(glBindTexture(GL_TEXTURE_2D, m_texture));
		glCheck(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_actualSize.x, m_actualSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_isRepeated ? GL_REPEAT : (textureEdgeClamp ? GLEXT_GL_CLAMP_TO_EDGE : GLEXT_GL_CLAMP)));
		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_isRepeated ? GL_REPEAT : (textureEdgeClamp ? GLEXT_GL_CLAMP_TO_EDGE : GLEXT_GL_CLAMP)));
		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_isSmooth ? GL_LINEAR : GL_NEAREST));
		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_isSmooth ? GL_LINEAR : GL_NEAREST));
		m_cacheId = getUniqueId();
#else
		priv::TextureSaver save;
		// Initialize the texture
		glCheck(glBindTexture(GL_TEXTURE_2D, m_texture));
		glCheck(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_actualSize.x, m_actualSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_isRepeated ? GL_REPEAT : GL_CLAMP_TO_EDGE));
		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_isRepeated ? GL_REPEAT : GL_CLAMP_TO_EDGE));
		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_isSmooth ? GL_LINEAR : GL_NEAREST));
		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_isSmooth ? GL_LINEAR : GL_NEAREST));
#endif
		return true;
	}


	////////////////////////////////////////////////////////////
	bool Texture::loadFromFile(const std::string& filename, const IntRect& area)
	{
		Image image;
		return image.loadFromFile(filename) && loadFromImage(image, area);
	}


	////////////////////////////////////////////////////////////
	bool Texture::loadFromMemory(const void* data, std::size_t size, const IntRect& area)
	{
		Image image;
		return image.loadFromMemory(data, size) && loadFromImage(image, area);
	}


	////////////////////////////////////////////////////////////
	bool Texture::loadFromStream(std::istream&  stream, const IntRect& area)
	{
		Image image;
		return image.loadFromStream(stream) && loadFromImage(image, area);
	}


	////////////////////////////////////////////////////////////
	bool Texture::loadFromImage(const Image& image, const IntRect& area)
	{
		// Retrieve the image size
		int width = static_cast<int>(image.getSize().x);
		int height = static_cast<int>(image.getSize().y);

		// Load the entire image if the source area is either empty or contains the whole image
		if (area.width == 0 || (area.height == 0) ||
			((area.left <= 0) && (area.top <= 0) && (area.width >= width) && (area.height >= height)))
		{
			// Load the entire image
			if (create(image.getSize().x, image.getSize().y))
			{
				update(image);

				// Force an OpenGL flush, so that the texture will appear updated
				// in all contexts immediately (solves problems in multi-threaded apps)
				glCheck(glFlush());

				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			// Load a sub-area of the image

			// Adjust the rectangle to the size of the image
			IntRect rectangle = area;
			if (rectangle.left   < 0) rectangle.left = 0;
			if (rectangle.top    < 0) rectangle.top = 0;
			if (rectangle.left + rectangle.width > width)  rectangle.width = width - rectangle.left;
			if (rectangle.top + rectangle.height > height) rectangle.height = height - rectangle.top;

			// Create the texture and upload the pixels
			if (create(rectangle.width, rectangle.height))
			{
				// Make sure that the current texture binding will be preserved
				priv::TextureSaver save;

				// Copy the pixels to the texture, row by row
				const uint8_t* pixels = image.getPixelsPtr() + 4 * (rectangle.left + (width * rectangle.top));
				glCheck(glBindTexture(GL_TEXTURE_2D, m_texture));
				for (int i = 0; i < rectangle.height; ++i)
				{
					glCheck(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, i, rectangle.width, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixels));
					pixels += 4 * width;
				}

				// Force an OpenGL flush, so that the texture will appear updated
				// in all contexts immediately (solves problems in multi-threaded apps)
				glCheck(glFlush());

				return true;
			}
			else
			{
				return false;
			}
		}
	}


	////////////////////////////////////////////////////////////
	Vector2u Texture::getSize() const
	{
		return m_size;
	}


	////////////////////////////////////////////////////////////
	Image Texture::copyToImage() const
	{
		// Easy case: empty texture
		if (!m_texture)
			return Image();
#if 0
		ensureGlContext();
#endif
		// Make sure that the current texture binding will be preserved
		priv::TextureSaver save;

		// Create an array of pixels
		std::vector<uint8_t> pixels(m_size.x * m_size.y * 4);

#ifdef SFML_OPENGL_ES

		// OpenGL ES doesn't have the glGetTexImage function, the only way to read
		// from a texture is to bind it to a FBO and use glReadPixels
		GLuint frameBuffer = 0;
		glCheck(GLEXT_glGenFramebuffers(1, &frameBuffer));
		if (frameBuffer)
		{
			GLint previousFrameBuffer;
			glCheck(glGetIntegerv(GLEXT_GL_FRAMEBUFFER_BINDING, &previousFrameBuffer));

			glCheck(GLEXT_glBindFramebuffer(GLEXT_GL_FRAMEBUFFER, frameBuffer));
			glCheck(GLEXT_glFramebufferTexture2D(GLEXT_GL_FRAMEBUFFER, GLEXT_GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0));
			glCheck(glReadPixels(0, 0, m_size.x, m_size.y, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]));
			glCheck(GLEXT_glDeleteFramebuffers(1, &frameBuffer));

			glCheck(GLEXT_glBindFramebuffer(GLEXT_GL_FRAMEBUFFER, previousFrameBuffer));
		}

#else

		if ((m_size == m_actualSize) && !m_pixelsFlipped)
		{
			// Texture is not padded nor flipped, we can use a direct copy
			glCheck(glBindTexture(GL_TEXTURE_2D, m_texture));
			glCheck(glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]));
		}
		else
		{
			// Texture is either padded or flipped, we have to use a slower algorithm

			// All the pixels will first be copied to a temporary array
			std::vector<uint8_t> allPixels(m_actualSize.x * m_actualSize.y * 4);
			glCheck(glBindTexture(GL_TEXTURE_2D, m_texture));
			glCheck(glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, &allPixels[0]));

			// Then we copy the useful pixels from the temporary array to the final one
			const uint8_t* src = &allPixels[0];
			uint8_t* dst = &pixels[0];
			int srcPitch = m_actualSize.x * 4;
			int dstPitch = m_size.x * 4;

			// Handle the case where source pixels are flipped vertically
			if (m_pixelsFlipped)
			{
				src += srcPitch * (m_size.y - 1);
				srcPitch = -srcPitch;
			}

			for (unsigned int i = 0; i < m_size.y; ++i)
			{
				std::memcpy(dst, src, dstPitch);
				src += srcPitch;
				dst += dstPitch;
			}
		}

#endif // SFML_OPENGL_ES

		// Create the image
		Image image;
		image.create(m_size.x, m_size.y, pixels.data());

		return image;
	}


	////////////////////////////////////////////////////////////
	void Texture::update(const uint8_t* pixels)
	{
		// Update the whole texture
		update(pixels, m_size.x, m_size.y, 0, 0);
	}


	////////////////////////////////////////////////////////////
	void Texture::update(const uint8_t* pixels, unsigned int width, unsigned int height, unsigned int x, unsigned int y)
	{
		assert(x + width <= m_size.x);
		assert(y + height <= m_size.y);

		if (pixels && m_texture)
		{
#if 0
			ensureGlContext();
#endif
			// Make sure that the current texture binding will be preserved
			priv::TextureSaver save;

			// Copy pixels from the given array to the texture
			glCheck(glBindTexture(GL_TEXTURE_2D, m_texture));
			glCheck(glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels));
			m_pixelsFlipped = false;
			m_cacheId = getUniqueId();
		}
	}


	////////////////////////////////////////////////////////////
	void Texture::update(const Image& image)
	{
		// Update the whole texture
		update(image.getPixelsPtr(), image.getSize().x, image.getSize().y, 0, 0);
	}


	////////////////////////////////////////////////////////////
	void Texture::update(const Image& image, unsigned int x, unsigned int y)
	{
		update(image.getPixelsPtr(), image.getSize().x, image.getSize().y, x, y);
	}

#if 0
	////////////////////////////////////////////////////////////
	void Texture::update(const Window& window) { update(window, 0, 0); }


	////////////////////////////////////////////////////////////
	void Texture::update(const Window& window, unsigned int x, unsigned int y)
	{
		assert(x + window.getSize().x <= m_size.x);
		assert(y + window.getSize().y <= m_size.y);

		if (m_texture && window.setActive(true))
		{
			// Make sure that the current texture binding will be preserved
			priv::TextureSaver save;

			// Copy pixels from the back-buffer to the texture
			glCheck(glBindTexture(GL_TEXTURE_2D, m_texture));
			glCheck(glCopyTexSubImage2D(GL_TEXTURE_2D, 0, x, y, 0, 0, window.getSize().x, window.getSize().y));
			m_pixelsFlipped = true;
			m_cacheId = getUniqueId();
		}
	}

#endif
	////////////////////////////////////////////////////////////
	void Texture::setSmooth(bool smooth)
	{
		if (smooth != m_isSmooth)
		{
			m_isSmooth = smooth;

			if (m_texture)
			{
#if 0
				ensureGlContext();

				// Make sure that the current texture binding will be preserved
#endif
				priv::TextureSaver save;

				glCheck(glBindTexture(GL_TEXTURE_2D, m_texture));
				glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_isSmooth ? GL_LINEAR : GL_NEAREST));
				glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_isSmooth ? GL_LINEAR : GL_NEAREST));
			}
		}
	}


	////////////////////////////////////////////////////////////
	bool Texture::isSmooth() const
	{
		return m_isSmooth;
	}


	////////////////////////////////////////////////////////////
	void Texture::setRepeated(bool repeated)
	{
		if (repeated != m_isRepeated)
		{
			m_isRepeated = repeated;

			if (m_texture)
			{
#if 0
				ensureGlContext();

				// Make sure that the current texture binding will be preserved
				priv::TextureSaver save;

				static bool textureEdgeClamp = GLEXT_texture_edge_clamp || GLEXT_EXT_texture_edge_clamp;

				if (!m_isRepeated && !textureEdgeClamp)
				{
					static bool warned = false;

					if (!warned)
					{
						err() << "OpenGL extension SGIS_texture_edge_clamp unavailable" << std::endl;
						err() << "Artifacts may occur along texture edges" << std::endl;
						err() << "Ensure that hardware acceleration is enabled if available" << std::endl;

						warned = true;
					}
				}		
				glCheck(glBindTexture(GL_TEXTURE_2D, m_texture));
				glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_isRepeated ? GL_REPEAT : (textureEdgeClamp ? GLEXT_GL_CLAMP_TO_EDGE : GLEXT_GL_CLAMP)));
				glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_isRepeated ? GL_REPEAT : (textureEdgeClamp ? GLEXT_GL_CLAMP_TO_EDGE : GLEXT_GL_CLAMP)));
#else
				priv::TextureSaver save;
				glCheck(glBindTexture(GL_TEXTURE_2D, m_texture));
				glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_isRepeated ? GL_REPEAT : GL_CLAMP_TO_EDGE));
				glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_isRepeated ? GL_REPEAT : GL_CLAMP_TO_EDGE));
#endif
			}
		}
	}


	////////////////////////////////////////////////////////////
	bool Texture::isRepeated() const
	{
		return m_isRepeated;
	}


	////////////////////////////////////////////////////////////
	void Texture::bind(const Texture* texture, CoordinateType coordinateType)
	{
#if 0
		ensureGlContext();
#endif
		if (texture && texture->m_texture)
		{
			// Bind the texture
			glCheck(glBindTexture(GL_TEXTURE_2D, texture->m_texture));

			// Check if we need to define a special texture matrix
			if ((coordinateType == Pixels) || texture->m_pixelsFlipped)
			{
				GLfloat matrix[16] = { 1.f, 0.f, 0.f, 0.f,
					0.f, 1.f, 0.f, 0.f,
					0.f, 0.f, 1.f, 0.f,
					0.f, 0.f, 0.f, 1.f };

				// If non-normalized coordinates (= pixels) are requested, we need to
				// setup scale factors that convert the range [0 .. size] to [0 .. 1]
				if (coordinateType == Pixels)
				{
					matrix[0] = 1.f / texture->m_actualSize.x;
					matrix[5] = 1.f / texture->m_actualSize.y;
				}

				// If pixels are flipped we must invert the Y axis
				if (texture->m_pixelsFlipped)
				{
					matrix[5] = -matrix[5];
					matrix[13] = static_cast<float>(texture->m_size.y) / texture->m_actualSize.y;
				}

				// Load the matrix
				glCheck(glMatrixMode(GL_TEXTURE));
				glCheck(glLoadMatrixf(matrix));

				// Go back to model-view mode (sf::RenderTarget relies on it)
				glCheck(glMatrixMode(GL_MODELVIEW));
			}
		}
		else
		{
			// Bind no texture
			glCheck(glBindTexture(GL_TEXTURE_2D, 0));

			// Reset the texture matrix
			glCheck(glMatrixMode(GL_TEXTURE));
			glCheck(glLoadIdentity());

			// Go back to model-view mode (sf::RenderTarget relies on it)
			glCheck(glMatrixMode(GL_MODELVIEW));
		}
	}


	////////////////////////////////////////////////////////////
	unsigned int Texture::getMaximumSize()
	{
		static unsigned int size = checkMaximumTextureSize();
		return size;
	}


	////////////////////////////////////////////////////////////
	Texture& Texture::operator =(const Texture& right)
	{
		Texture temp(right);

		std::swap(m_size, temp.m_size);
		std::swap(m_actualSize, temp.m_actualSize);
		std::swap(m_texture, temp.m_texture);
		std::swap(m_isSmooth, temp.m_isSmooth);
		std::swap(m_isRepeated, temp.m_isRepeated);
		std::swap(m_pixelsFlipped, temp.m_pixelsFlipped);
		std::swap(m_fboAttachment, temp.m_fboAttachment);
		m_cacheId = getUniqueId();

		return *this;
	}


	////////////////////////////////////////////////////////////
	unsigned int Texture::getNativeHandle() const
	{
		return m_texture;
	}


	////////////////////////////////////////////////////////////
	unsigned int Texture::getValidSize(unsigned int size)
	{
#if 0
		ensureGlContext();

		// Make sure that extensions are initialized
		priv::ensureExtensionsInit();

		if (GLEXT_texture_non_power_of_two)
		{
#endif
			// If hardware supports NPOT textures, then just return the unmodified size
			return size;
#if 0
		}
		else
		{
			// If hardware doesn't support NPOT textures, we calculate the nearest power of two
			unsigned int powerOfTwo = 1;
			while (powerOfTwo < size)
				powerOfTwo *= 2;

			return powerOfTwo;
		}
#endif
	}
	////////////////////////////////////////////////////////////
	RenderTarget::RenderTarget() :
		m_defaultView(),
		m_view(),
		m_cache()
	{
		m_cache.glStatesSet = false;
	}


	////////////////////////////////////////////////////////////
	RenderTarget::~RenderTarget()
	{
	}


	////////////////////////////////////////////////////////////
	void RenderTarget::clear(const Color& color)
	{
		if (activate(true))
		{
			// Unbind texture to fix RenderTexture preventing clear
			applyTexture(NULL);

			glCheck(glClearColor(color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f));
			glCheck(glClear(GL_COLOR_BUFFER_BIT));
		}
	}


	////////////////////////////////////////////////////////////
	void RenderTarget::setView(const View& view)
	{
		m_view = view;
		m_cache.viewChanged = true;
	}


	////////////////////////////////////////////////////////////
	const View& RenderTarget::getView() const
	{
		return m_view;
	}


	////////////////////////////////////////////////////////////
	const View& RenderTarget::getDefaultView() const
	{
		return m_defaultView;
	}


	////////////////////////////////////////////////////////////
	IntRect RenderTarget::getViewport(const View& view) const
	{
		float width = static_cast<float>(getSize().x);
		float height = static_cast<float>(getSize().y);
		const FloatRect& viewport = view.getViewport();

		return IntRect(static_cast<int>(0.5f + width  * viewport.left),
			static_cast<int>(0.5f + height * viewport.top),
			static_cast<int>(0.5f + width  * viewport.width),
			static_cast<int>(0.5f + height * viewport.height));
	}


	////////////////////////////////////////////////////////////
	Vector2f RenderTarget::mapPixelToCoords(const Vector2i& point) const
	{
		return mapPixelToCoords(point, getView());
	}


	////////////////////////////////////////////////////////////
	Vector2f RenderTarget::mapPixelToCoords(const Vector2i& point, const View& view) const
	{
		// First, convert from viewport coordinates to homogeneous coordinates
		Vector2f normalized;
		IntRect viewport = getViewport(view);
		normalized.x = -1.f + 2.f * (point.x - viewport.left) / viewport.width;
		normalized.y = 1.f - 2.f * (point.y - viewport.top) / viewport.height;

		// Then transform by the inverse of the view matrix
		return view.getInverseTransform().transformPoint(normalized);
	}


	////////////////////////////////////////////////////////////
	Vector2i RenderTarget::mapCoordsToPixel(const Vector2f& point) const
	{
		return mapCoordsToPixel(point, getView());
	}


	////////////////////////////////////////////////////////////
	Vector2i RenderTarget::mapCoordsToPixel(const Vector2f& point, const View& view) const
	{
		// First, transform the point by the view matrix
		Vector2f normalized = view.getTransform().transformPoint(point);

		// Then convert to viewport coordinates
		Vector2i pixel;
		IntRect viewport = getViewport(view);
		pixel.x = static_cast<int>((normalized.x + 1.f) / 2.f * viewport.width + viewport.left);
		pixel.y = static_cast<int>((-normalized.y + 1.f) / 2.f * viewport.height + viewport.top);

		return pixel;
	}


	////////////////////////////////////////////////////////////
	void RenderTarget::draw(const Drawable& drawable, const RenderStates& states)
	{
		drawable.draw(*this, states);
	}


	////////////////////////////////////////////////////////////
	void RenderTarget::draw(const Vertex* vertices, std::size_t vertexCount,
		PrimitiveType type, const RenderStates& states)
	{
		// Nothing to draw?
		if (!vertices || (vertexCount == 0))
			return;

		// GL_QUADS is unavailable on OpenGL ES
#ifdef SFML_OPENGL_ES
		if (type == Quads)
		{
			err() << "sf::Quads primitive type is not supported on OpenGL ES platforms, drawing skipped" << std::endl;
			return;
		}
#define GL_QUADS 0
#endif

		if (activate(true))
		{
			// First set the persistent OpenGL states if it's the very first call
			if (!m_cache.glStatesSet)
				resetGLStates();

			// Check if the vertex count is low enough so that we can pre-transform them
			bool useVertexCache = (vertexCount <= StatesCache::VertexCacheSize);
			if (useVertexCache)
			{
				// Pre-transform the vertices and store them into the vertex cache
				for (std::size_t i = 0; i < vertexCount; ++i)
				{
					Vertex& vertex = m_cache.vertexCache[i];
					vertex.position = states.transform * vertices[i].position;
					vertex.color = vertices[i].color;
					vertex.texCoords = vertices[i].texCoords;
				}

				// Since vertices are transformed, we must use an identity transform to render them
				if (!m_cache.useVertexCache)
					applyTransform(Transform::Identity);
			}
			else
			{
				applyTransform(states.transform);
			}

			// Apply the view
			if (m_cache.viewChanged)
				applyCurrentView();

			// Apply the blend mode
			if (states.blendMode != m_cache.lastBlendMode)
				applyBlendMode(states.blendMode);

			// Apply the texture
			uint64_t textureId = states.texture ? states.texture->m_cacheId : 0;
			if (textureId != m_cache.lastTextureId)
				applyTexture(states.texture);

			// Apply the shader
			if (states.shader)
				applyShader(states.shader);

			// If we pre-transform the vertices, we must use our internal vertex cache
			if (useVertexCache)
			{
				// ... and if we already used it previously, we don't need to set the pointers again
				if (!m_cache.useVertexCache)
					vertices = m_cache.vertexCache;
				else
					vertices = NULL;
			}

			// Setup the pointers to the vertices' components
			if (vertices)
			{
				const char* data = reinterpret_cast<const char*>(vertices);
				glCheck(glVertexPointer(2, GL_FLOAT, sizeof(Vertex), data + 0));
				glCheck(glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), data + 8));
				glCheck(glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), data + 12));
			}

			// Find the OpenGL primitive type
			static const GLenum modes[] = { GL_POINTS, GL_LINES, GL_LINE_STRIP, GL_TRIANGLES,
				GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_QUADS };
			GLenum mode = modes[type];

			// Draw the primitives
			glCheck(glDrawArrays(mode, 0, vertexCount));

			// Unbind the shader, if any
			if (states.shader)
				applyShader(NULL);

			// If the texture we used to draw belonged to a RenderTexture, then forcibly unbind that texture.
			// This prevents a bug where some drivers do not clear RenderTextures properly.
			if (states.texture && states.texture->m_fboAttachment)
				applyTexture(NULL);

			// Update the cache
			m_cache.useVertexCache = useVertexCache;
		}
	}


	////////////////////////////////////////////////////////////
	void RenderTarget::pushGLStates()
	{
		if (activate(true))
		{
#ifdef SFML_DEBUG
			// make sure that the user didn't leave an unchecked OpenGL error
			GLenum error = glGetError();
			if (error != GL_NO_ERROR)
			{
				err() << "OpenGL error (" << error << ") detected in user code, "
					<< "you should check for errors with glGetError()"
					<< std::endl;
			}
#endif

#ifndef SFML_OPENGL_ES
			glCheck(glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS));
			glCheck(glPushAttrib(GL_ALL_ATTRIB_BITS));
#endif
			glCheck(glMatrixMode(GL_MODELVIEW));
			glCheck(glPushMatrix());
			glCheck(glMatrixMode(GL_PROJECTION));
			glCheck(glPushMatrix());
			glCheck(glMatrixMode(GL_TEXTURE));
			glCheck(glPushMatrix());
		}

		resetGLStates();
	}


	////////////////////////////////////////////////////////////
	void RenderTarget::popGLStates()
	{
		if (activate(true))
		{
			glCheck(glMatrixMode(GL_PROJECTION));
			glCheck(glPopMatrix());
			glCheck(glMatrixMode(GL_MODELVIEW));
			glCheck(glPopMatrix());
			glCheck(glMatrixMode(GL_TEXTURE));
			glCheck(glPopMatrix());
#ifndef SFML_OPENGL_ES
			glCheck(glPopClientAttrib());
			glCheck(glPopAttrib());
#endif
		}
	}


	////////////////////////////////////////////////////////////
	void RenderTarget::resetGLStates()
	{
		// Check here to make sure a context change does not happen after activate(true)
		bool shaderAvailable = false; // Shader::isAvailable();

		if (activate(true))
		{
			// Make sure that extensions are initialized
			priv::ensureExtensionsInit();

			// Make sure that the texture unit which is active is the number 0
			if (GLEXT_multitexture)
			{
				glCheck(GLEXT_glClientActiveTexture(GLEXT_GL_TEXTURE0));
				glCheck(GLEXT_glActiveTexture(GLEXT_GL_TEXTURE0));
			}

			// Define the default OpenGL states
			glCheck(glDisable(GL_CULL_FACE));
			glCheck(glDisable(GL_LIGHTING));
			glCheck(glDisable(GL_DEPTH_TEST));
			glCheck(glDisable(GL_ALPHA_TEST));
			glCheck(glEnable(GL_TEXTURE_2D));
			glCheck(glEnable(GL_BLEND));
			glCheck(glMatrixMode(GL_MODELVIEW));
			glCheck(glEnableClientState(GL_VERTEX_ARRAY));
			glCheck(glEnableClientState(GL_COLOR_ARRAY));
			glCheck(glEnableClientState(GL_TEXTURE_COORD_ARRAY));
			m_cache.glStatesSet = true;

			// Apply the default SFML states
			applyBlendMode(BlendAlpha);
			applyTransform(Transform::Identity);
			applyTexture(NULL);
			if (shaderAvailable)
				applyShader(NULL);

			m_cache.useVertexCache = false;

			// Set the default view
			setView(getView());
		}
	}


	////////////////////////////////////////////////////////////
	void RenderTarget::initialize()
	{
		// Setup the default and current views
		m_defaultView.reset(FloatRect(0, 0, static_cast<float>(getSize().x), static_cast<float>(getSize().y)));
		m_view = m_defaultView;

		// Set GL states only on first draw, so that we don't pollute user's states
		m_cache.glStatesSet = false;
	}


	////////////////////////////////////////////////////////////
	void RenderTarget::applyCurrentView()
	{
		// Set the viewport
		IntRect viewport = getViewport(m_view);
		int top = getSize().y - (viewport.top + viewport.height);
		glCheck(glViewport(viewport.left, top, viewport.width, viewport.height));

		// Set the projection matrix
		glCheck(glMatrixMode(GL_PROJECTION));
		glCheck(glLoadMatrixf(m_view.getTransform().getMatrix()));

		// Go back to model-view mode
		glCheck(glMatrixMode(GL_MODELVIEW));

		m_cache.viewChanged = false;
	}


	////////////////////////////////////////////////////////////
	void RenderTarget::applyBlendMode(const BlendMode& mode)
	{
		// Apply the blend mode, falling back to the non-separate versions if necessary
		if (GLEXT_blend_func_separate)
		{
			glCheck(GLEXT_glBlendFuncSeparate(
				factorToGlConstant(mode.colorSrcFactor), factorToGlConstant(mode.colorDstFactor),
				factorToGlConstant(mode.alphaSrcFactor), factorToGlConstant(mode.alphaDstFactor)));
		}
		else
		{
			glCheck(glBlendFunc(
				factorToGlConstant(mode.colorSrcFactor),
				factorToGlConstant(mode.colorDstFactor)));
		}

		if (GLEXT_blend_minmax && GLEXT_blend_subtract)
		{
			if (GLEXT_blend_equation_separate)
			{
				glCheck(GLEXT_glBlendEquationSeparate(
					equationToGlConstant(mode.colorEquation),
					equationToGlConstant(mode.alphaEquation)));
			}
			else
			{
				glCheck(GLEXT_glBlendEquation(equationToGlConstant(mode.colorEquation)));
			}
		}
		else if ((mode.colorEquation != BlendMode::Add) || (mode.alphaEquation != BlendMode::Add))
		{
			static bool warned = false;

			if (!warned)
			{
				err() << "OpenGL extension EXT_blend_minmax and/or EXT_blend_subtract unavailable" << std::endl;
				err() << "Selecting a blend equation not possible" << std::endl;
				err() << "Ensure that hardware acceleration is enabled if available" << std::endl;

				warned = true;
			}
		}

		m_cache.lastBlendMode = mode;
	}


	////////////////////////////////////////////////////////////
	void RenderTarget::applyTransform(const Transform& transform)
	{
		// No need to call glMatrixMode(GL_MODELVIEW), it is always the
		// current mode (for optimization purpose, since it's the most used)
		glCheck(glLoadMatrixf(transform.getMatrix()));
	}


	////////////////////////////////////////////////////////////
	void RenderTarget::applyTexture(const Texture* texture)
	{
		Texture::bind(texture, Texture::Pixels);

		m_cache.lastTextureId = texture ? texture->m_cacheId : 0;
	}


	////////////////////////////////////////////////////////////
	void RenderTarget::applyShader(const Shader* shader)
	{
		//Shader::bind(shader);
	}

};