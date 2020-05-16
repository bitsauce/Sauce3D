// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#pragma once

#include <Sauce/Common.h>
#include <Sauce/Graphics/Shader.h>
#include <Sauce/Graphics/Texture.h>
#include <Sauce/Graphics/RenderTarget.h>
#include <Sauce/Graphics/VertexBuffer.h>
#include <Sauce/Graphics/BlendState.h>
#include <Sauce/Graphics/TextureRegion.h>

BEGIN_SAUCE_NAMESPACE

SAUCE_FORWARD_DECLARE(Window);

class Vertex;
class RenderTarget2D;
class VertexBuffer;
class IndexBuffer;

/**
 * Primitive types.
 * For rendering different kinds of primitives.
 */
enum class PrimitiveType : uint32
{
	Points,        ///< Points. 1 vertex per primitive.
	Lines,         ///< Lines. 2 vertex per primitive.
	LineStrip,     ///< Line strip. 2 vertex for the first primitive, 1 for the next.
	LineLoop,      ///< Line loop. n lines
	Triangles,     ///< Triangles. 3 vertex per primitive.
	TriangleStrip, ///< Triangle strip. 3 vertex for the first primitive, 1 vertex for the next.
	TriangleFan    ///< Triangle fan. 3 vertex for the first primitive, 1 vertex for the next.
};

/**
 * Clearing buffer mask.
 * Used for selecting how to clear the backbuffer.
 */
enum class BufferMask : uint32
{
	Color   = 1 << 0,          ///< %Color buffer
	Depth   = 1 << 1,          ///< Depth buffer
	Stencil = 1 << 2           ///< Stencil buffer
};
ENUM_CLASS_ADD_BITWISE_OPERATORS(BufferMask);

/**
  * Graphics capabilites.
  * For enabling and disabling certain rendering options.
  */
enum class Capability : uint32
{
	Blend,                  ///< Back buffer blending
	DepthTest,              ///< Depth testing
	FaceCulling,            ///< Back face culling
	LineSmoothing,          ///< Smooth lines
	PolygonSmoothing,       ///< Smooth polygons
	Multisample,            ///< Multisample?
	Texture1D,              ///< 1D textures
	Texture2D,              ///< 2D textures
	Texture3D,              ///< 3D textures
	Vsync,                  ///< VSync
	Wireframe               ///< Wireframe
};

struct SAUCE_API GraphicsContextDesc : public SauceObjectDesc
{
	WindowRef owningWindow;
};

/**
 * Abstract class representing a rendering system
 */
class SAUCE_API GraphicsContext : public SauceObject
{
	friend class Game;

public:
	SAUCE_REF_TYPE(GraphicsContext);

	GraphicsContext();
	virtual ~GraphicsContext();

	virtual bool initialize(GraphicsContextDesc graphicsContextDesc) = 0;
	static GraphicsContext* CreateImpl();

public:
	static GraphicsContextRef GetContext();
	static GraphicsBackend GetEffectiveBackend();

	struct State
	{
		State()
			: width(0)
			, height(0)
			, texture(nullptr)
			, shader(nullptr)
			, blendState(BlendPreset::AlphaBlend)
			, renderTarget(nullptr)
		{
			transformationMatrixStack.push(Matrix4());
		}

		uint width;
		uint height;
		Texture2DRef texture;
		ShaderRef shader;
		BlendState blendState;
		RenderTarget2DRef renderTarget;
		stack<Matrix4> transformationMatrixStack;
		Matrix4 projectionMatrix;
	};

	// TODO: For every set*, add a push*/pop* which uses the state stack
	void pushState();
	void popState();

	/**
	 * Sets a render target. This means that everything drawn after this function call
	 * will be rendered to the given \p renderTarget instead of the screen.
	 * TODO: Implement a render target stack.
	 * \param renderTarget The target buffer to render to.
	 */
	void pushRenderTarget(RenderTarget2DRef renderTarget);
	void popRenderTarget();

	/**
	 * Get current render target.
	 */
	RenderTarget2DRef getRenderTarget() const
	{
		return m_stateStack.top().renderTarget;
	}
	
	/**
	 * Pushes a matrix onto the transformation matrix stack.
	 * The matrix is pushed as follows \code stack.push_back(stack.top() * mat) \endcode
	 * \param mat Matrix to push on the stack.
	 */
	void pushMatrix(const Matrix4 &mat);

	/**
	 * Pops the top matrix from the transformation matrix stack.
	 */
	bool popMatrix();

	/**
	* Gets the top transformation matrix
	*/
	Matrix4 topMatrix() const;
	
	/**
	 *	Clear matrix stack
	 */
	void clearMatrixStack();

	/**
	 * Set texture. Every primitive rendered after this will have \p texture
	 * applied to it.
	 * \param texture Texture to apply to the primitives.
	 */
	void setTexture(Texture2DRef texture);

	/**
	 * Gets the current texture.
	 */
	Texture2DRef getTexture() const;

	/**
	 * Set shader. Every vertex and fragment rendered after this will
	 * have the effect of \p shader applied to them.
	 * \param shader Shader to render the primitves with.
	 */
	void setShader(ShaderRef shader);

	/**
	 * Returns the current shader.
	 */
	ShaderRef getShader() const;

	/**
	 * Set blend state. Every pixel rendered after this will use a 
	 * formula defined by \p blendState to blend new pixels with the back buffer.
	 * \param blendState The blend state to render with.
	 */
	void setBlendState(const BlendState &blendState);

	/**
	 * Returns the current blend state.
	 */
	BlendState getBlendState();

	/**
	 * Resizes the viewport (the area of the screen rendered to).
	 * \param w Width of the viewport in pixels
	 * \param h Height of the viewport in pixels
	 */
	void setSize(const uint w, const uint h);

	/**
	 * Set projection matrix
	 */
	void setProjectionMatrix(const Matrix4 matrix);

	/**
	 * Returns the width of the viewport.
	 */
	uint getWidth() const
	{
		return m_stateStack.top().width;
	}

	/**
	 * Returns the height of the viewport.
	 */
	uint getHeight() const
	{
		return m_stateStack.top().height;
	}

	/**
	* Returns the size of the viewport.
	*/
	Vector2I getSize() const
	{
		return Vector2I(m_stateStack.top().width, m_stateStack.top().height);
	}

	/**
	 * Renders a rectangle.
	 * \param rect Rectangle to render.
	 * \param color %Color of the rectangle.
	 * \param textureRegion Texture region of the rectangle.
	 */
	void drawRectangle(const Rect<float> &rect, const Color &color = Color::White, const TextureRegion &textureRegion = TextureRegion());

	/**
	 * Renders a rectangle.
	 * \param pos Top-left corner of the rectangle.
	 * \param size Size of the rectangle.
	 * \param color %Color of the rectangle.
	 * \param textureRegion Texture region of the rectangle.
	 */
	void drawRectangle(const Vector2F &pos, const Vector2F &size, const Color &color = Color::White, const TextureRegion &textureRegion = TextureRegion());

	/**
	 * Renders a rectangle.
	 * \param x Left x position of the rectangle.
	 * \param y Top y position of the rectangle.
	 * \param width Width of the rectangle.
	 * \param height Height of the rectangle.
	 * \param color %Color of the rectangle.
	 * \param textureRegion Texture region of the rectangle.
	 */
	void drawRectangle(const float x, const float y, const float width, const float height, const Color &color = Color::White, const TextureRegion &textureRegion = TextureRegion());

	/**
	* Renders a rectangle outline.
	* \param rect Rectangle to render.
	* \param color %Color of the rectangle.
	* \param textureRegion Texture region of the rectangle.
	*/
	void drawRectangleOutline(const Rect<float> &rect, const Color &color = Color::White, const TextureRegion &textureRegion = TextureRegion());

	/**
	* Renders a rectangle outline.
	* \param pos Top-left corner of the rectangle.
	* \param size Size of the rectangle.
	* \param color %Color of the rectangle.
	* \param textureRegion Texture region of the rectangle.
	*/
	void drawRectangleOutline(const Vector2F &pos, const Vector2F &size, const Color &color = Color::White, const TextureRegion &textureRegion = TextureRegion());

	/**
	* Renders a rectangle outline.
	* \param x Left x position of the rectangle.
	* \param y Top y position of the rectangle.
	* \param width Width of the rectangle.
	* \param height Height of the rectangle.
	* \param color %Color of the rectangle.
	* \param textureRegion Texture region of the rectangle.
	*/
	void drawRectangleOutline(const float x, const float y, const float width, const float height, const Color &color = Color::White, const TextureRegion &textureRegion = TextureRegion());

	/**
	 * Renders a circle.
	 * \param pos Center of the circle.
	 * \param radius Radius of the circle.
	 * \param segments Number of triangle segments to divide the circle into.
	 * \param color %Color of the circle.
	 */
	void drawCircleGradient(const Vector2F &pos, const float radius, const uint segments, const Color &center = Color::White, const Color &outer = Color::White);
	void drawCircle(const Vector2F &pos, const float radius, const uint segments, const Color &color = Color::White);

	/**
	 * Renders a circle.
	 * \param x Center x position of the circle.
	 * \param y Center y position of the circle.
	 * \param radius Radius of the circle.
	 * \param segments Number of triangle segments to divide the circle into.
	 * \param color %Color of the circle.
	 */
	void drawCircleGradient(const float x, const float y, const float radius, const uint segments, const Color &center = Color::White, const Color &outer = Color::White);
	void drawCircle(const float x, const float y, const float radius, const uint segments, const Color &color = Color::White);

	/**
	 * Draws an arrow from a starting point to an ending point.
	 * \param x0 Starting x-coordinate
	 * \param y0 Starting y-coordinate
	 * \param x1 Ending x-coordinate
	 * \param y1 Ending y-coordinate
	 * \param color %Color of the arrow
	 */
	void drawArrow(const float x0, const float y0, const float x1, const float y1, const float arrowHeadSize = 10.0f, const Color &color = Color::White);
	void drawArrow(const Vector2F p0, const Vector2F p1, const float arrowHeadSize = 10.0f, const Color &color = Color::White) { drawArrow(p0.x, p0.y, p1.x, p1.y, arrowHeadSize, color); }

	/**
	 * Fast way to draw with a temporary vertex array
	 */
	VertexArray& getTempVertexArray(const uint32 vertexCount);

public:
	/************************************************
	 *  Pure virtual backend dependent functions    *
	 ************************************************/

	/**
	 * Enables the capability \p cap.
	 * \param cap Capability to enable.
	 */
	virtual void enable(const Capability cap) = 0;

	/**
	 * Disables the capability \p cap.
	 * \param cap Capability to disable.
	 */
	virtual void disable(const Capability cap) = 0;

	/**
	 * Returns true if capability \p cap is enabled
	 */
	virtual bool isEnabled(const Capability cap) = 0;

	/**
	 * Enable scissor testing
	 */
	virtual void enableScissor(const int x, const int y, const int w, const int h) = 0;
	
	/**
	 * Disable scissor testing
	 */
	virtual void disableScissor() = 0;

	/**
	 * Set rendering point size
	 */
	virtual void setPointSize(const float pointSize) = 0;
	
	/**
	 * Set rendering line width
	 */
	virtual void setLineWidth(const float lineWidth) = 0;
	
	/**
	 * Set the size of the viewport
	 */
	virtual void setViewportSize(const uint w, const uint h) = 0;

	/**
	 * Clears the back buffer using \p mask.
	 * \param mask Decides what channels in the back buffer to clear.
	 * \param fillColor Decides what value to clear to.
	 */
	virtual void clear(const uint32 clearMask, const Color& clearColor = Color(0, 0, 0, 0), const double clearDepth = 1.0, const int32 clearStencil = 0) = 0;

	/**
	 * Saves a screen shot of the back buffer to \p path as a PNG file.
	 * \param path Screen shot destination path
	 */
	virtual void saveScreenshot(string path) = 0;
	
	/**
	 * Create matricies
	 */
	virtual Matrix4 createOrtographicMatrix(const float left, const float right, const float top, const float bottom, const float n = -1.0f, const float f = 1.0f) const = 0;
	virtual Matrix4 createPerspectiveMatrix(const float fov, const float aspectRatio, const float zNear, const float zFar) const = 0;
	virtual Matrix4 createLookAtMatrix(const Vector3F &position, const Vector3F &fwd) const = 0;
	
	/**
	 * Renders an indexed primitive to the screen.
	 * \param type Types of primitives to render.
	 * \param vertices Array of vertices to render.
	 * \param vertexCount Number of vertices to render.
	 * \param indices Array of indices.
	 * \param indexCount Number of indices.
	 */
	virtual void drawIndexedPrimitives(const PrimitiveType type, const VertexArray& vertices, const uint vertexCount, const uint* indices, const uint indexCount) = 0;

	/**
	 * Renders an indexed primitive to the screen using vertex and index buffers.
	 * \param type Types of primitives to render.
	 * \param vbo Vertex buffer object.
	 * \param ibo Index buffer object.
	 */
	virtual void drawIndexedPrimitives(const PrimitiveType type, const VertexBufferRef vertexBuffer, const IndexBufferRef indexBuffer) = 0;

	/**
	 * Renders primitives to the screen.
	 * \param type Types of primitives to render.
	 * \param vertices Array of vertices to render.
	 * \param vertexCount Number of vertices to render.
	 */
	virtual void drawPrimitives(const PrimitiveType type, const VertexArray& vertices, const uint vertexCount) = 0;

	/**
	 * Renders primitives to the screen.
	 * \param type Types of primitives to render.
	 * \param vbo Vertex buffer object.
	 */
	virtual void drawPrimitives(const PrimitiveType type, const VertexBufferRef vertexBuffer) = 0;

protected:
	/**
	 * Texture2D internal API
	 */
	friend class Texture2D;
	void texture2D_getDeviceObject(Texture2DRef texture, Texture2DDeviceObject*& outShaderDeviceObject);
	virtual void texture2D_createDeviceObject(Texture2DDeviceObject*& textureDeviceObject, const string& deviceObjectName) = 0;
	virtual void texture2D_destroyDeviceObject(Texture2DDeviceObject*& outTextureDeviceObject) = 0;
	virtual void texture2D_copyToGPU(Texture2DDeviceObject* textureDeviceObject, const PixelFormat pixelFormat, const uint32 width, const uint32 height, uint8* textureData) = 0;
	virtual void texture2D_copyToCPUReadable(Texture2DDeviceObject* textureDeviceObject, uint8** outTextureData) = 0;
	virtual void texture2D_updateSubregion(Texture2DDeviceObject* textureDeviceObject, const uint32 x, const uint32 y, const uint32 subRegionWidth, const uint32 subRegionHeight, uint8* textureData) = 0;
	virtual void texture2D_updateFiltering(Texture2DDeviceObject* textureDeviceObject, const TextureFiltering filtering) = 0;
	virtual void texture2D_updateWrapping(Texture2DDeviceObject* textureDeviceObject, const TextureWrapping wrapping) = 0;
	virtual void texture2D_clearTexture(Texture2DDeviceObject* textureDeviceObject) = 0;
	
	/**
	 * Shader internal API
	 */
	friend class Shader;
	void shader_getDeviceObject(ShaderRef shader, ShaderDeviceObject*& outShaderDeviceObject);
	virtual void shader_createDeviceObject(ShaderDeviceObject*& shaderDeviceObject, const string& deviceObjectName) = 0;
	virtual void shader_destroyDeviceObject(ShaderDeviceObject*& shaderDeviceObject) = 0;
	virtual void shader_compileShader(ShaderDeviceObject* shaderDeviceObject, const string& vsSource, const string& psSource, const string& gsSource) = 0;
	virtual void shader_setUniform(ShaderDeviceObject* shaderDeviceObject, const string& uniformName, const Datatype datatype, const uint32 numComponentsPerElement, const uint32 numElements, const void* data) = 0;
	virtual void shader_setSampler2D(ShaderDeviceObject* shaderDeviceObject, const string& uniformName, Texture2DRef texture) = 0;

	/**
	 * RenderTarget2D internal API
	 */
	friend class RenderTarget2D;
	void renderTarget2D_getDeviceObject(RenderTarget2DRef renderTarget, RenderTarget2DDeviceObject*& outRenderTargetDeviceObject);
	virtual void renderTarget2D_createDeviceObject(RenderTarget2DDeviceObject*& outRenderTargetDeviceObject, const string& deviceObjectName) = 0;
	virtual void renderTarget2D_destroyDeviceObject(RenderTarget2DDeviceObject*& outRenderTargetDeviceObject) = 0;
	virtual void renderTarget2D_initializeRenderTarget(RenderTarget2DDeviceObject* renderTargetDeviceObject, const Texture2DRef* targetTextures, const uint32 targetCount) = 0;
	virtual void renderTarget2D_bindRenderTarget(RenderTarget2DDeviceObject* renderTargetDeviceObject) = 0;

	/**
	 * VertexBuffer internal API
	 */
	friend class VertexBuffer;
	void vertexBuffer_getDeviceObject(VertexBufferRef vertexBuffer, VertexBufferDeviceObject*& outVertexBufferDeviceObject);
	virtual void vertexBuffer_createDeviceObject(VertexBufferDeviceObject*& outVertexBufferDeviceObject, const string& deviceObjectName) = 0;
	virtual void vertexBuffer_destroyDeviceObject(VertexBufferDeviceObject*& outVertexBufferDeviceObject) = 0;
	virtual void vertexBuffer_initializeVertexBuffer(VertexBufferDeviceObject* vertexBufferDeviceObject, const BufferUsage bufferUsage, const VertexArray& vertices, const uint32 vertexCount) = 0;
	virtual void vertexBuffer_modifyVertexBuffer(VertexBufferDeviceObject* vertexBufferDeviceObject, const uint32 startIndex, const VertexArray& vertices, const uint32 vertexCount) = 0;
	virtual void vertexBuffer_bindVertexBuffer(VertexBufferDeviceObject* vertexBufferDeviceObject) = 0;

	/**
	 * IndexBuffer internal API
	 */
	friend class IndexBuffer;
	void indexBuffer_getDeviceObject(IndexBufferRef indexBuffer, IndexBufferDeviceObject*& outIndexBufferDeviceObject);
	virtual void indexBuffer_createDeviceObject(IndexBufferDeviceObject*& outIndexBufferDeviceObject, const string& deviceObjectName) = 0;
	virtual void indexBuffer_destroyDeviceObject(IndexBufferDeviceObject*& outIndexBufferDeviceObject) = 0;
	virtual void indexBuffer_initializeIndexBuffer(IndexBufferDeviceObject* indexBufferDeviceObject, const BufferUsage bufferUsage, const uint32* indices, const uint32 indexCount) = 0;
	virtual void indexBuffer_modifyIndexBuffer(IndexBufferDeviceObject* indexBufferDeviceObject, const uint32 startIndex, const uint32* indices, const uint32 indexCount) = 0;
	virtual void indexBuffer_bindIndexBuffer(IndexBufferDeviceObject* indexBufferDeviceObject) = 0;

protected:
	WindowRef m_owningWindow;

	stack<State> m_stateStack;
	State* m_currentState;

	/** We keep a list of vertices for when we might need it */
	VertexArray m_tempVertices;

	static ShaderRef s_defaultShader;
	static Texture2DRef s_defaultTexture;
	static RefType s_this;
	static GraphicsBackend s_effectiveBackend;
};
SAUCE_REF_TYPE_TYPEDEFS(GraphicsContext);

END_SAUCE_NAMESPACE
