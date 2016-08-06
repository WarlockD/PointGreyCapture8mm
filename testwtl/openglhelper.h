#pragma once

void glCheckError(const char* file, unsigned int line, const char* expression);
#define glCheck(expr) do { expr; glCheckError(__FILE__, __LINE__, #expr); } while (false)

namespace sf {
	// ripped off from sfml
////////////////////////////////////////////////////////////
/// \brief Utility template class for manipulating
///        2-dimensional vectors
///
////////////////////////////////////////////////////////////
	template <typename T>
	class Vector2
	{
	public:
		T x; ///< X coordinate of the vector
		T y; ///< Y coordinate of the vector
	public:
		inline Vector2() : x(0), y(0) {}
		inline Vector2(T X, T Y) : x(X), y(Y) {}
		template <typename U> explicit Vector2(const Vector2<U>& vector);
	};
	template <typename T> template <typename U> inline Vector2<T>::Vector2(const Vector2<U>& vector) : x(static_cast<T>(vector.x)), y(static_cast<T>(vector.y)) { }
	template <typename T> inline Vector2<T> operator -(const Vector2<T>& right) { return Vector2<T>(-right.x, -right.y); }

	template <typename T> inline Vector2<T>& operator +=(Vector2<T>& left, const Vector2<T>& right) { left.x += right.x; left.y += right.y; return left; }
	template <typename T> inline Vector2<T>& operator -=(Vector2<T>& left, const Vector2<T>& right) { left.x -= right.x; left.y -= right.y; return left; }

	template <typename T> inline Vector2<T> operator +(const Vector2<T>& left, const Vector2<T>& right) { return Vector2<T>(left.x + right.x, left.y + right.y); }
	template <typename T> inline Vector2<T> operator -(const Vector2<T>& left, const Vector2<T>& right) { return Vector2<T>(left.x - right.x, left.y - right.y); }
	template <typename T> inline Vector2<T> operator *(const Vector2<T>& left, T right) { return Vector2<T>(left.x * right, left.y * right); }
	template <typename T> inline Vector2<T> operator *(T left, const Vector2<T>& right) { return Vector2<T>(left * right.x, left * right.y); }
	template <typename T> inline Vector2<T>& operator *=(Vector2<T>& left, T right) { left.x *= right; left.y *= right; return left; }
	template <typename T> inline Vector2<T> operator /(const Vector2<T>& left, T right) { return Vector2<T>(left.x / right, left.y / right); }
	template <typename T> inline Vector2<T>& operator /=(Vector2<T>& left, T right) { left.x /= right; left.y /= right; return left; }
	template <typename T> inline bool operator ==(const Vector2<T>& left, const Vector2<T>& right) { return (left.x == right.x) && (left.y == right.y); }
	template <typename T> inline bool operator !=(const Vector2<T>& left, const Vector2<T>& right) { return (left.x != right.x) || (left.y != right.y); }

	// Define the most common types
	typedef Vector2<int>          Vector2i;
	typedef Vector2<unsigned int> Vector2u;
	typedef Vector2<float>        Vector2f;


	template <typename T>
	class Rect
	{
	public:
		T left;   ///< Left coordinate of the rectangle
		T top;    ///< Top coordinate of the rectangle
		T width;  ///< Width of the rectangle
		T height; ///< Height of the rectangle
	public:
		Rect() : left(0), top(0), width(0), height(0) {}
		Rect(T rectLeft, T rectTop, T rectWidth, T rectHeight) : left(rectLeft), top(rectTop), width(rectWidth), height(rectHeight) {}
		Rect(const Vector2<T>& position, const Vector2<T>& size) : left(position.x), top(position.y), width(size.x), height(size.y) {}
		template <typename U> explicit Rect(const Rect<U>& rectangle) : left(static_cast<T>(rectangle.left)), top(static_cast<T>(rectangle.top)), width(static_cast<T>(rectangle.width)), height(static_cast<T>(rectangle.height)) {}
		bool contains(T x, T y) const {
			// Rectangles with negative dimensions are allowed, so we must handle them correctly

			// Compute the real min and max of the rectangle on both axes
			T minX = std::min(left, static_cast<T>(left + width));
			T maxX = std::max(left, static_cast<T>(left + width));
			T minY = std::min(top, static_cast<T>(top + height));
			T maxY = std::max(top, static_cast<T>(top + height));

			return (x >= minX) && (x < maxX) && (y >= minY) && (y < maxY);
		}
		bool contains(const Vector2<T>& point) const { return contains(point.x, point.y); }
		bool intersects(const Rect<T>& rectangle) const {
			Rect<T> intersection;
			return intersects(rectangle, intersection);
		}
		bool intersects(const Rect<T>& rectangle, Rect<T>& intersection) const {
			// Rectangles with negative dimensions are allowed, so we must handle them correctly

			// Compute the min and max of the first rectangle on both axes
			T r1MinX = std::min(left, static_cast<T>(left + width));
			T r1MaxX = std::max(left, static_cast<T>(left + width));
			T r1MinY = std::min(top, static_cast<T>(top + height));
			T r1MaxY = std::max(top, static_cast<T>(top + height));

			// Compute the min and max of the second rectangle on both axes
			T r2MinX = std::min(rectangle.left, static_cast<T>(rectangle.left + rectangle.width));
			T r2MaxX = std::max(rectangle.left, static_cast<T>(rectangle.left + rectangle.width));
			T r2MinY = std::min(rectangle.top, static_cast<T>(rectangle.top + rectangle.height));
			T r2MaxY = std::max(rectangle.top, static_cast<T>(rectangle.top + rectangle.height));

			// Compute the intersection boundaries
			T interLeft = std::max(r1MinX, r2MinX);
			T interTop = std::max(r1MinY, r2MinY);
			T interRight = std::min(r1MaxX, r2MaxX);
			T interBottom = std::min(r1MaxY, r2MaxY);

			// If the intersection is valid (positive non zero area), then there is an intersection
			if ((interLeft < interRight) && (interTop < interBottom))
			{
				intersection = Rect<T>(interLeft, interTop, interRight - interLeft, interBottom - interTop);
				return true;
			}
			else
			{
				intersection = Rect<T>(0, 0, 0, 0);
				return false;
			}
		}

	};
	template <typename T> inline bool operator ==(const Rect<T>& left, const Rect<T>& right) { return (left.left == right.left) && (left.width == right.width) && (left.top == right.top) && (left.height == right.height); }
	template <typename T> inline bool operator !=(const Rect<T>& left, const Rect<T>& right) { return !(left == right); }


	typedef Rect<int>   IntRect;
	typedef Rect<float> FloatRect;

	////////////////////////////////////////////////////////////
	/// \brief Define a 3x3 transform matrix
	///
	////////////////////////////////////////////////////////////
	class Transform
	{
	private:
		float m_matrix[16]; ///< 4x4 matrix defining the transformation
	public:
		static const Transform Identity; ///< The identity transform (does nothing)
		Transform();
		Transform(float a00, float a01, float a02,
			float a10, float a11, float a12,
			float a20, float a21, float a22);
		const float* getMatrix() const { return m_matrix; }
		Transform getInverse() const;
		Vector2f transformPoint(float x, float y) const;
		Vector2f transformPoint(const Vector2f& point) const { return transformPoint(point.x, point.y); }
		FloatRect transformRect(const FloatRect& rectangle) const;
		Transform& combine(const Transform& transform);
		Transform& translate(float x, float y);
		Transform& translate(const Vector2f& offset) { return translate(offset.x, offset.y); }
		Transform& rotate(float angle);
		Transform& rotate(float angle, float centerX, float centerY);
		Transform& rotate(float angle, const Vector2f& center) { return rotate(angle, center.x, center.y); }
		Transform& scale(float scaleX, float scaleY);
		Transform& scale(float scaleX, float scaleY, float centerX, float centerY);
		Transform& scale(const Vector2f& factors) { return scale(factors.x, factors.y); }
		Transform& scale(const Vector2f& factors, const Vector2f& center) { return scale(factors.x, factors.y, center.x, center.y); }
	};
	Transform operator *(const Transform& left, const Transform& right) { return Transform(left).combine(right); }
	Transform& operator *=(Transform& left, const Transform& right) { return left.combine(right); }
	Vector2f operator *(const Transform& left, const Vector2f& right) { return left.transformPoint(right); }

	struct BlendMode
	{
		enum Factor
		{
			Zero,             ///< (0, 0, 0, 0)
			One,              ///< (1, 1, 1, 1)
			SrcColor,         ///< (src.r, src.g, src.b, src.a)
			OneMinusSrcColor, ///< (1, 1, 1, 1) - (src.r, src.g, src.b, src.a)
			DstColor,         ///< (dst.r, dst.g, dst.b, dst.a)
			OneMinusDstColor, ///< (1, 1, 1, 1) - (dst.r, dst.g, dst.b, dst.a)
			SrcAlpha,         ///< (src.a, src.a, src.a, src.a)
			OneMinusSrcAlpha, ///< (1, 1, 1, 1) - (src.a, src.a, src.a, src.a)
			DstAlpha,         ///< (dst.a, dst.a, dst.a, dst.a)
			OneMinusDstAlpha  ///< (1, 1, 1, 1) - (dst.a, dst.a, dst.a, dst.a)
		};
		enum Equation
		{
			Add,     ///< Pixel = Src * SrcFactor + Dst * DstFactor
			Subtract ///< Pixel = Src * SrcFactor - Dst * DstFactor
		};
		Factor   colorSrcFactor; ///< Source blending factor for the color channels
		Factor   colorDstFactor; ///< Destination blending factor for the color channels
		Equation colorEquation;  ///< Blending equation for the color channels
		Factor   alphaSrcFactor; ///< Source blending factor for the alpha channel
		Factor   alphaDstFactor; ///< Destination blending factor for the alpha channel
		Equation alphaEquation;  ///< Blending equation for the alpha channel
		BlendMode();
		BlendMode::BlendMode(Factor sourceFactor, Factor destinationFactor, Equation blendEquation = Add);
		BlendMode::BlendMode(Factor colorSourceFactor, Factor colorDestinationFactor, Equation colorBlendEquation, Factor alphaSourceFactor, Factor alphaDestinationFactor, Equation alphaBlendEquation);
	};
	bool operator==(const BlendMode& left, const BlendMode& right) ;
	bool operator!=(const BlendMode& left, const BlendMode& right) ;

	extern const BlendMode BlendAlpha;
	extern const BlendMode BlendAdd;
	extern const BlendMode BlendMultiply;
	extern const BlendMode BlendNone;

	class Color {
	public:
		uint8_t r; ///< Red component
		uint8_t g; ///< Green component
		uint8_t b; ///< Blue component
		uint8_t a; ///< Alpha (opacity) component
		static const Color Black;       ///< Black predefined color
		static const Color White;       ///< White predefined color
		static const Color Red;         ///< Red predefined color
		static const Color Green;       ///< Green predefined color
		static const Color Blue;        ///< Blue predefined color
		static const Color Yellow;      ///< Yellow predefined color
		static const Color Magenta;     ///< Magenta predefined color
		static const Color Cyan;        ///< Cyan predefined color
		static const Color Transparent; ///< Transparent (black) predefined color
										////////////////////////////////////////////////////////////
										/// \brief Default constructor
										///
										/// Constructs an opaque black color. It is equivalent to
										/// sf::Color(0, 0, 0, 255).
										///
										////////////////////////////////////////////////////////////
		inline Color() :r(0), g(0), b(0), a(255) {}
		inline Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255) :r(red), g(green), b(blue), a(alpha) {}
		inline explicit Color(uint32_t color) :r((color & 0xff000000) >> 24),g((color & 0x00ff0000) >> 16),b((color & 0x0000ff00) >> 8),a((color & 0x000000ff) >> 0){}
		inline uint32_t toInteger() const { return (r << 24) | (g << 16) | (b << 8) | a; }
	};
	bool operator ==(const Color& left, const Color& right) { return (left.r == right.r) && (left.g == right.g) && (left.b == right.b) && (left.a == right.a); }
	bool operator !=(const Color& left, const Color& right) { return !(left == right); }
	Color operator +(const Color& left, const Color& right);
	Color operator -(const Color& left, const Color& right);
	Color operator *(const Color& left, const Color& right);
	Color& operator +=(Color& left, const Color& right) { return left = left + right; }
	Color& operator -=(Color& left, const Color& right) { return left = left - right; }
	Color& operator *=(Color& left, const Color& right) { return left = left * right; }

	// Basic Image handling

	class Image 
	{
		std::vector<uint8_t> _pixels;
		Vector2u _size;
	public:
		Image() :_pixels(),_size(0,0)  {}
		void create(unsigned int width, unsigned int height, const Color& color = Color(0, 0, 0));
		void create(unsigned int width, unsigned int height, const uint8_t* pixels);
		void create(unsigned int width, unsigned int height, std::vector<uint8_t>&& move);
		bool loadFromFile(const std::string& filename);
		bool loadFromMemory(const void* data, std::size_t size);
		bool loadFromStream(std::istream& stream);
		bool saveToFile(const std::string& filename) const;
		const Vector2u& getSize() const { return _size; }
		void createMaskFromColor(const Color& color, uint8_t alpha = 0);
		void copy(const Image& source, unsigned int destX, unsigned int destY, const IntRect& sourceRect = IntRect(0, 0, 0, 0), bool applyAlpha = false);
		void setPixel(unsigned int x, unsigned int y, const Color& color);
		Color getPixel(unsigned int x, unsigned int y) const;
		const uint8_t* getPixelsPtr() const { return _pixels.size() == 0 ? nullptr : _pixels.data(); }
		void flipHorizontally();
		void flipVertically();
	};


	// basic vertex
	class  Vertex
	{
	public:
		Vector2f  position;  ///< 2D position of the vertex
		Color     color;     ///< Color of the vertex
		Vector2f  texCoords; ///< Coordinates of the texture's pixel to map to the vertex
		Vertex::Vertex() :position(0, 0),color(255, 255, 255), texCoords(0, 0) {}
		Vertex::Vertex(const Vector2f& thePosition) :position(thePosition),color(255, 255, 255),texCoords(0, 0){}
		Vertex::Vertex(const Vector2f& thePosition, const Color& theColor) :position(thePosition),color(theColor),texCoords(0, 0){}
		Vertex::Vertex(const Vector2f& thePosition, const Vector2f& theTexCoords) :position(thePosition),color(255, 255, 255),texCoords(theTexCoords){}
		Vertex::Vertex(const Vector2f& thePosition, const Color& theColor, const Vector2f& theTexCoords) :position(thePosition),color(theColor),texCoords(theTexCoords){}
	};
	class View
	{
	private:
		Vector2f          m_center;              ///< Center of the view, in scene coordinates
		Vector2f          m_size;                ///< Size of the view, in scene coordinates
		float             m_rotation;            ///< Angle of rotation of the view rectangle, in degrees
		FloatRect         m_viewport;            ///< Viewport rectangle, expressed as a factor of the render-target's size
		mutable Transform m_transform;           ///< Precomputed projection transform corresponding to the view
		mutable Transform m_inverseTransform;    ///< Precomputed inverse projection transform corresponding to the view
		mutable bool      m_transformUpdated;    ///< Internal state telling if the transform needs to be updated
		mutable bool      m_invTransformUpdated; ///< Internal state telling if the inverse transform needs to be updated
	public:
		View();
		explicit View(const FloatRect& rectangle);
		View(const Vector2f& center, const Vector2f& size);
		void setCenter(float x, float y);
		void setCenter(const Vector2f& center);
		void setSize(float width, float height);
		void setSize(const Vector2f& size);
		void setRotation(float angle);
		void setViewport(const FloatRect& viewport);
		void reset(const FloatRect& rectangle);
		const Vector2f& getCenter() const;
		const Vector2f& getSize() const;
		float getRotation() const;
		const FloatRect& getViewport() const;
		void move(float offsetX, float offsetY);
		void move(const Vector2f& offset);
		void rotate(float angle);
		void zoom(float factor);
		const Transform& getTransform() const;
		const Transform& getInverseTransform() const;
	};
	class Shader {}; // filler
	class  Texture 
	{
	public:
		enum CoordinateType
		{
			Normalized, ///< Texture coordinates in range [0 .. 1]
			Pixels      ///< Texture coordinates in range [0 .. size]
		};

	public:
		Texture();
		Texture(const Texture& copy);
		~Texture();
		bool create(unsigned int width, unsigned int height);
		
		bool loadFromFile(const std::string& filename, const IntRect& area = IntRect());
		bool loadFromMemory(const void* data, std::size_t size, const IntRect& area = IntRect());
		bool loadFromStream(std::istream& stream, const IntRect& area = IntRect());
		bool loadFromImage(const Image& image, const IntRect& area = IntRect());
		Vector2u getSize() const;
		Image copyToImage() const;
		void update(const uint8_t* pixels);
		void update(const uint8_t* pixels, unsigned int width, unsigned int height, unsigned int x, unsigned int y);
		void update(const Image& image);
		void update(const Image& image, unsigned int x, unsigned int y);
		//void update(const Window& window);
		//void update(const Window& window, unsigned int x, unsigned int y);
		void setSmooth(bool smooth);
		bool isSmooth() const;
		void setRepeated(bool repeated);
		bool isRepeated() const;
		Texture& operator =(const Texture& right);
		unsigned int getNativeHandle() const;
		static void bind(const Texture* texture, CoordinateType coordinateType = Normalized);
		static unsigned int getMaximumSize();

	private:

		friend class RenderTexture;
		friend class RenderTarget;

		////////////////////////////////////////////////////////////
		/// \brief Get a valid image size according to hardware support
		///
		/// This function checks whether the graphics driver supports
		/// non power of two sizes or not, and adjusts the size
		/// accordingly.
		/// The returned size is greater than or equal to the original size.
		///
		/// \param size size to convert
		///
		/// \return Valid nearest size (greater than or equal to specified size)
		///
		////////////////////////////////////////////////////////////
		static unsigned int getValidSize(unsigned int size);

		////////////////////////////////////////////////////////////
		// Member data
		////////////////////////////////////////////////////////////
		Vector2u     m_size;          ///< Public texture size
		Vector2u     m_actualSize;    ///< Actual texture size (can be greater than public size because of padding)
		unsigned int m_texture;       ///< Internal texture identifier
		bool         m_isSmooth;      ///< Status of the smooth filter
		bool         m_isRepeated;    ///< Is the texture in repeat mode?
		mutable bool m_pixelsFlipped; ///< To work around the inconsistency in Y orientation
		bool         m_fboAttachment; ///< Is this texture owned by a framebuffer object?
		uint64_t       m_cacheId;       ///< Unique number that identifies the texture to the render target's cache
	};

	class RenderStates {
	public:
		static const RenderStates Default; ///< Special instance holding the default render states
		BlendMode      blendMode; ///< Blending mode
		Transform      transform; ///< Transform
		const Texture* texture;   ///< Texture
		const Shader*  shader;    ///< Shader
		RenderStates() : blendMode(BlendAlpha), transform(), texture(nullptr), shader(nullptr) {}
		RenderStates(const BlendMode& theBlendMode, const Transform& theTransform, const Texture* theTexture, const Shader* theShader) :
			blendMode(theBlendMode), transform(theTransform), texture(theTexture), shader(theShader) {}
		RenderStates(const BlendMode& theBlendMode) : blendMode(theBlendMode), transform(), texture(nullptr), shader(nullptr) {}
		RenderStates(const Transform& theTransform) : blendMode(BlendAlpha), transform(theTransform), texture(nullptr), shader(nullptr) {}
		RenderStates(const Texture* theTexture) : blendMode(BlendAlpha), transform(), texture(theTexture), shader(nullptr) {}
		RenderStates(const Shader* theShader) : blendMode(BlendAlpha), transform(), texture(nullptr), shader(theShader) {}
	};

	class  Drawable
	{
	public:
		virtual ~Drawable() {}
	protected:
		friend class RenderTarget;
		virtual void draw(RenderTarget& target, RenderStates states) const = 0;
	};

	enum PrimitiveType
	{
		Points,         ///< List of individual points
		Lines,          ///< List of individual lines
		LinesStrip,     ///< List of connected lines, a point uses the previous point to form a line
		Triangles,      ///< List of individual triangles
		TrianglesStrip, ///< List of connected triangles, a point uses the two previous points to form a triangle
		TrianglesFan,   ///< List of connected triangles, a point uses the common center and the previous point to form a triangle
		Quads           ///< List of individual quads (deprecated, don't work with OpenGL ES)
	};


	// stripped of all the fancy shader stuff
	class  RenderTarget
	{

		////////////////////////////////////////////////////////////
		/// \brief Render states cache
		///
		////////////////////////////////////////////////////////////
		struct StatesCache
		{
			enum { VertexCacheSize = 4 };

			bool      glStatesSet;    ///< Are our internal GL states set yet?
			bool      viewChanged;    ///< Has the current view changed since last draw?
			BlendMode lastBlendMode;  ///< Cached blending mode
			uint64_t    lastTextureId;  ///< Cached texture
			bool      useVertexCache; ///< Did we previously use the vertex cache?
			Vertex    vertexCache[VertexCacheSize]; ///< Pre-transformed vertices cache
		};

		////////////////////////////////////////////////////////////
		// Member data
		////////////////////////////////////////////////////////////
		View        m_defaultView; ///< Default view
		View        m_view;        ///< Current view
		StatesCache m_cache;       ///< Render states cache
	public:
		RenderTarget(const RenderTarget& copy) = delete;
		RenderTarget& operator=(const RenderTarget& copy) = delete; // not copyable
		virtual ~RenderTarget();
		void clear(const Color& color = Color(0, 0, 0, 255));
		void setView(const View& view);
		const View& getView() const;
		const View& getDefaultView() const;
		IntRect getViewport(const View& view) const;
		Vector2f mapPixelToCoords(const Vector2i& point) const;
		Vector2f mapPixelToCoords(const Vector2i& point, const View& view) const;
		Vector2i mapCoordsToPixel(const Vector2f& point) const;
		Vector2i mapCoordsToPixel(const Vector2f& point, const View& view) const;
		void draw(const Drawable& drawable, const RenderStates& states = RenderStates::Default);
		void draw(const Vertex* vertices, std::size_t vertexCount,
			PrimitiveType type, const RenderStates& states = RenderStates::Default);
		virtual Vector2u getSize() const = 0;
		void pushGLStates();
		void popGLStates();
		void resetGLStates();
	protected:
		RenderTarget();
		void initialize();
	private:
		void applyCurrentView();
		void applyBlendMode(const BlendMode& mode);
		void applyTransform(const Transform& transform);
		void applyTexture(const Texture* texture);
		void applyShader(const Shader* shader);
		virtual bool activate(bool active) = 0;


	};

};