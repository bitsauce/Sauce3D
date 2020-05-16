// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#pragma once

#include <Sauce/Common.h>
#include <Sauce/Graphics/Vertex.h>

BEGIN_SAUCE_NAMESPACE

SAUCE_FORWARD_DECLARE(GraphicsContext);

class Vertex;

enum class BufferUsage : uint32
{
	Static,
	Dynamic,
	Stream
};

/*********************************************************************
**	Vertex buffer													**
**********************************************************************/

struct SAUCE_API VertexBufferDeviceObject
{
	virtual ~VertexBufferDeviceObject() { }
	
	BufferUsage  bufferUsage  = BufferUsage::Static;
	VertexFormat vertexFormat = VertexFormat();
	uint32       vertexCount  = 0;
};

struct SAUCE_API VertexBufferDesc : public GraphicsDeviceObjectDesc
{
	BufferUsage  bufferUsage = BufferUsage::Static;
	VertexArray* vertices    = nullptr;
	uint32       vertexCount = 0;
};

class SAUCE_API VertexBuffer : public SauceObject<VertexBuffer, VertexBufferDesc>
{
	friend class GraphicsContext;

public:
	VertexBuffer();
	virtual ~VertexBuffer();

	bool initialize(DescType) override;

	void modifyData(const uint32 startIndex, const VertexArray& vertices, const uint vertexCount);

	VertexFormat getVertexFormat() const;
	uint32 getVertexCount() const;

private:
	GraphicsContextRef m_graphicsContext;
	VertexBufferDeviceObject* m_deviceObject;
};
SAUCE_TYPEDEFS(VertexBuffer);

/*********************************************************************
**	Index buffer													**
**********************************************************************/

struct SAUCE_API IndexBufferDeviceObject
{
	virtual ~IndexBufferDeviceObject() { }

	BufferUsage bufferUsage = BufferUsage::Static;
	uint32      indexCount  = 0;
};

struct SAUCE_API IndexBufferDesc : public GraphicsDeviceObjectDesc
{
	BufferUsage bufferUsage = BufferUsage::Static;
	uint32*     indices     = nullptr;
	uint32      indexCount  = 0;
};

class SAUCE_API IndexBuffer : public SauceObject<IndexBuffer, IndexBufferDesc>
{
	friend class GraphicsContext;

public:
	IndexBuffer();
	virtual ~IndexBuffer();

	bool initialize(DescType) override;

	uint32 getIndexCount() const;

private:
	GraphicsContextRef m_graphicsContext;
	IndexBufferDeviceObject* m_deviceObject;
};
SAUCE_TYPEDEFS(IndexBuffer);

END_SAUCE_NAMESPACE
