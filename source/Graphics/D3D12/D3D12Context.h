// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#pragma once

#include <Sauce/Common.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <wrl.h>

#include <Graphics/D3D12/d3dx12.h>

using Microsoft::WRL::ComPtr;

BEGIN_SAUCE_NAMESPACE

struct ImmediateVertexBuffer
{
	ComPtr<ID3D12Resource>   vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
};

class SAUCE_API D3D12Context final : public GraphicsContext
{
	friend class GraphicsContext;

private:
	D3D12Context();
	~D3D12Context();

	bool initialize(DescType) override;

	/** Pipeline state object handling */
	ComPtr<ID3D12PipelineState> createOrFindPSO(const VertexFormat& vertexFormat, ShaderDeviceObject** shaderDeviceObject);

	/** Command queue handling */
	void executeDrawCommandList(ShaderDeviceObject* shaderDeviceObject, ComPtr<ID3D12PipelineState> pso, const D3D12_VERTEX_BUFFER_VIEW& vertexBufferView);
	void flushCommandQueue();

	/** Command queue synchronization objects */
	ComPtr<ID3D12Fence> m_commandQueueFence;
	HANDLE m_commandQueueFenceEvent;
	UINT64 m_commandQueueFenceValue;
	UINT m_frameIndex;


	/** Immediate draw vertex buffer */
	//ComPtr<ID3D12Resource>   m_immediateDrawVertexBuffer;
	//D3D12_VERTEX_BUFFER_VIEW m_immediateDrawVertexBufferView;
	//uint32                   m_immediateDrawVertexBufferSize;

	unordered_map<uint32, vector<ImmediateVertexBuffer>> m_availableImmediateDrawVertexBuffers;
	unordered_map<uint32, vector<ImmediateVertexBuffer>> m_immediateDrawVertexBuffers;

	//vector<ComPtr<ID3D12Resource>> m_immediateDrawVertexBuffer;
	//vector<D3D12_VERTEX_BUFFER_VIEW> m_immediateDrawVertexBufferView;


	vector<ComPtr<ID3D12DescriptorHeap>> m_inflightCbvSrvUavHeaps;

	struct PSOKey
	{
		ShaderRef    shader;
		VertexFormat vertexFormat;

		string getHash() const;
	};

	unordered_map<string, ComPtr<ID3D12PipelineState>> m_availablePSOs;

	

public:
	void enable(const Capability cap) override;
	void disable(const Capability cap) override;
	bool isEnabled(const Capability cap) override;

	void enableScissor(const int x, const int y, const int w, const int h) override;
	void disableScissor() override;

	void setPointSize(const float pointSize) override;
	void setLineWidth(const float lineWidth) override;
	void setViewportSize(const uint w, const uint h) override;
	void clear(const uint32 clearMask, const Color& clearColor, const double clearDepth, const int32 clearStencil) override;

	void saveScreenshot(string filePath) override;

	Matrix4 createOrtographicMatrix(const float left, const float right, const float top, const float bottom, const float n = -1.0f, const float f = 1.0f) const override;
	Matrix4 createPerspectiveMatrix(const float fov, const float aspectRatio, const float zNear, const float zFar) const override;
	Matrix4 createLookAtMatrix(const Vector3F& position, const Vector3F& fwd) const override;

	void drawIndexedPrimitives(const PrimitiveType type, const VertexArray& vertices, const uint vertexCount, const uint* indices, const uint indexCount) override;
	void drawIndexedPrimitives(const PrimitiveType type, const VertexBufferRef vertexBuffer, const IndexBufferRef indexBuffer) override;
	void drawPrimitives(const PrimitiveType type, const VertexArray& vertices, const uint vertexCount) override;
	void drawPrimitives(const PrimitiveType type, const VertexBufferRef vertexBuffer) override;

protected:
	virtual void presentFrame() override;

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
	struct ReflectionInfo
	{
		int32 numCbuffers = 0;
		int32 numSrvs = 0;
		int32 numSamplers = 0;

		uint32 cbufferBindPointMax = 0;
		uint32 srvBindPointMax = 0;
		uint32 samplerBindPointMax = 0;
	};

	ComPtr<ID3DBlob> compileAndReflect(ShaderDeviceObject* shaderDeviceObject, const string& shaderCode, const string& shaderMain, const string& shaderTarget, ReflectionInfo& outReflectionInfo);
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
};

END_SAUCE_NAMESPACE
