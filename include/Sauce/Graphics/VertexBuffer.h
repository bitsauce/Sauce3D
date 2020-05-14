// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#pragma once

#include <Sauce/Common.h>
#include <Sauce/Graphics/Vertex.h>

BEGIN_SAUCE_NAMESPACE

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

class SAUCE_API VertexBuffer : public SauceObject
{
	friend class GraphicsContext;
public:
	SAUCE_REF_TYPE(VertexBuffer);

	VertexBuffer();
	virtual ~VertexBuffer();

	bool initialize(VertexBufferDesc vertexBufferDesc);

	void modifyData(const uint32 startIndex, const VertexArray& vertices, const uint vertexCount);

	VertexFormat getVertexFormat() const;
	uint32 getVertexCount() const;

private:
	GraphicsContext* m_graphicsContext;
	VertexBufferDeviceObject* m_deviceObject;
};
SAUCE_REF_TYPE_TYPEDEFS(VertexBuffer);

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

class SAUCE_API IndexBuffer : public SauceObject
{
	friend class GraphicsContext;
public:
	SAUCE_REF_TYPE(IndexBuffer);

	IndexBuffer();
	virtual ~IndexBuffer();

	bool initialize(IndexBufferDesc indexBufferDesc);

	uint32 getIndexCount() const;

private:
	GraphicsContext* m_graphicsContext;
	IndexBufferDeviceObject* m_deviceObject;
};
SAUCE_REF_TYPE_TYPEDEFS(IndexBuffer);

END_SAUCE_NAMESPACE
