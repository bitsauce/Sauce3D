// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#pragma once

#include <Sauce/Common.h>
#include <Sauce/Graphics/BlendState.h>
#include <Sauce/Graphics/TextureRegion.h>

BEGIN_SAUCE_NAMESPACE

class Vertex;
class VertexFormat;
class RenderTarget2D;
class VertexBuffer;
class IndexBuffer;

class SAUCE_API OpenGLContext final : public GraphicsContext
{
	friend class GraphicsContext;
private:
	OpenGLContext(const int major, const int minor);
	~OpenGLContext();
	
	bool initialize(GraphicsContextDesc graphicsContextDesc) override;

	void setupContext();
	void setUniformsRecursive(const struct ShaderUniform* shaderUniform, const struct ShaderUniformLayout& uniformLayout, int32 &currentTextureTarget);
	void setupVertexAttributePointers(const VertexFormat& fmt);

public:
	void enable(const Capability cap) override;
	void disable(const Capability cap) override;
	bool isEnabled(const Capability cap) override;

	void enableScissor(const int x, const int y, const int w, const int h) override;
	void disableScissor() override;

	void setPointSize(const float pointSize) override;
	void setLineWidth(const float lineWidth) override;
	void setViewportSize(const uint w, const uint h) override;
	void clear(const uint32 clearMask, const Color &clearColor, const double clearDepth, const int32 clearStencil) override;

	void saveScreenshot(string filePath) override;

	Matrix4 createOrtographicMatrix(const float left, const float right, const float top, const float bottom, const float n = -1.0f, const float f = 1.0f) const override;
	Matrix4 createPerspectiveMatrix(const float fov, const float aspectRatio, const float zNear, const float zFar) const override;
	Matrix4 createLookAtMatrix(const Vector3F &position, const Vector3F &fwd) const override;

	void drawIndexedPrimitives(const PrimitiveType type, const VertexArray& vertices, const uint vertexCount, const uint* indices, const uint indexCount) override;
	void drawIndexedPrimitives(const PrimitiveType type, const VertexBufferRef vertexBuffer, const IndexBufferRef indexBuffer) override;
	void drawPrimitives(const PrimitiveType type, const VertexArray& vertices, const uint vertexCount) override;
	void drawPrimitives(const PrimitiveType type, const VertexBufferRef vertexBuffer) override;

	string getGLSLVersion() const;

protected:
	/**
	 * Texture2D internal API
	 */
	void texture2D_createDeviceObject(Texture2DDeviceObject*& outTextureDeviceObject, const string& deviceObjectName) override;
	void texture2D_destroyDeviceObject(Texture2DDeviceObject*& outTextureDeviceObject) override;
	void texture2D_copyToGPU(Texture2DDeviceObject* textureDeviceObject, const PixelFormat pixelFormat, const uint32 width, const uint32 height, uint8* textureData) override;
	void texture2D_copyToCPUReadable(Texture2DDeviceObject* textureDeviceObject, uint8** outTextureData) override;
	void texture2D_updateSubregion(Texture2DDeviceObject* textureDeviceObject, const uint32 x, const uint32 y, const uint32 subRegionWidth, const uint32 subRegionHeight, uint8* textureData) override;
	void texture2D_updateFiltering(Texture2DDeviceObject* textureDeviceObject, const TextureFiltering filtering) override;
	void texture2D_updateWrapping(Texture2DDeviceObject* textureDeviceObject, const TextureWrapping wrapping) override;
	void texture2D_clearTexture(Texture2DDeviceObject* textureDeviceObject) override;

	/**
	 * Shader internal API
	 */
	void shader_createDeviceObject(ShaderDeviceObject*& outShaderDeviceObject, const string& deviceObjectName) override;
	void shader_destroyDeviceObject(ShaderDeviceObject*& outShaderDeviceObject) override;
	void shader_compileShader(ShaderDeviceObject* shaderDeviceObject, const string& vsSource, const string& psSource, const string& gsSource) override;
	void shader_setUniform(ShaderDeviceObject* shaderDeviceObject, const string& uniformName, const Datatype datatype, const uint32 numComponentsPerElement, const uint32 numElements, const void* data) override;
	void shader_setSampler2D(ShaderDeviceObject* shaderDeviceObject, const string& uniformName, Texture2DRef texture) override;

	/**
	 * RenderTarget2D internal API
	 */
	void renderTarget2D_createDeviceObject(RenderTarget2DDeviceObject*& outRenderTargetDeviceObject, const string& deviceObjectName) override;
	void renderTarget2D_destroyDeviceObject(RenderTarget2DDeviceObject*& outRenderTargetDeviceObject) override;
	void renderTarget2D_initializeRenderTarget(RenderTarget2DDeviceObject* renderTargetDeviceObject, const Texture2DRef* targetTextures, const uint32 targetCount) override;
	void renderTarget2D_bindRenderTarget(RenderTarget2DDeviceObject* renderTargetDeviceObject) override;

	/**
	 * VertexBuffer internal API
	 */
	void vertexBuffer_createDeviceObject(VertexBufferDeviceObject*& outVertexBufferDeviceObject, const string& deviceObjectName) override;
	void vertexBuffer_destroyDeviceObject(VertexBufferDeviceObject*& outVertexBufferDeviceObject) override;
	void vertexBuffer_initializeVertexBuffer(VertexBufferDeviceObject* vertexBufferDeviceObject, const BufferUsage bufferUsage, const VertexArray& vertices, const uint32 vertexCount) override;
	void vertexBuffer_modifyVertexBuffer(VertexBufferDeviceObject* vertexBufferDeviceObject, const uint32 startIndex, const VertexArray& vertices, const uint32 vertexCount) override;
	void vertexBuffer_bindVertexBuffer(VertexBufferDeviceObject* vertexBufferDeviceObject) override;

	/**
	 * IndexBuffer internal API
	 */
	void indexBuffer_createDeviceObject(IndexBufferDeviceObject*& outIndexBufferDeviceObject, const string& deviceObjectName) override;
	void indexBuffer_destroyDeviceObject(IndexBufferDeviceObject*& outIndexBufferDeviceObject) override;
	void indexBuffer_initializeIndexBuffer(IndexBufferDeviceObject* indexBufferDeviceObject, const BufferUsage bufferUsage, const uint32* indices, const uint32 indexCount) override;
	void indexBuffer_modifyIndexBuffer(IndexBufferDeviceObject* indexBufferDeviceObject, const uint32 startIndex, const uint32* indices, const uint32 indexCount) override;
	void indexBuffer_bindIndexBuffer(IndexBufferDeviceObject* indexBufferDeviceObject) override;

private:
	const int m_majorVersion, m_minorVersion;
};

END_SAUCE_NAMESPACE
