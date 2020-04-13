// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#pragma once

#include <Sauce/Common.h>

BEGIN_SAUCE_NAMESPACE

class Vertex;

// TODO: Remove OpenGL dependency
enum class BufferUsage : uint32
{
	Static = GL_STATIC_DRAW,
	Dynamic = GL_DYNAMIC_DRAW,
	Stream = GL_STREAM_DRAW
};

/*********************************************************************
**	Vertex buffer													**
**********************************************************************/
class SAUCE_API VertexBuffer
{
	friend class OpenGLContext;
public:
	virtual ~VertexBuffer();

	// Add vertices and indices to the batch
	void setData(const Vertex *vertices, const uint vertexCount);
	char *getData() const;

	// Get vertex/vertex format/vertex count
	VertexFormat getVertexFormat() const;
	uint getSize() const { return m_size; }

protected:
	VertexBuffer(const BufferUsage usage);

	// Buffer ID
	GLuint m_id;

	// Vertex format
	VertexFormat m_format;

private:
	// Buffer usage
	BufferUsage m_usage;

	// Size
	uint m_size;
};

class SAUCE_API DynamicVertexBuffer : public VertexBuffer
{
public:
	DynamicVertexBuffer();
	DynamicVertexBuffer(const Vertex *vertices, const uint vertexCount);

	void modifyData(const uint startIdx, Vertex *vertex, const uint vertexCount);
};

class SAUCE_API StaticVertexBuffer : public VertexBuffer
{
public:
	StaticVertexBuffer();
	StaticVertexBuffer(const Vertex *vertices, const uint vertexCount);
};

/*********************************************************************
**	Index buffer													**
**********************************************************************/
class SAUCE_API IndexBuffer
{
	friend class OpenGLContext;
public:
	virtual ~IndexBuffer();

	// Add vertices and indices to the batch
	void setData(const uint *indices, const uint indexCount);
	char *getData() const;

	// Get size
	uint getSize() const { return m_size; }

protected:
	IndexBuffer(const BufferUsage usage);

	// Buffer ID
	GLuint m_id;

private:
	// Buffer type
	BufferUsage m_usage;

	// Size
	uint m_size;
};

class SAUCE_API DynamicIndexBuffer : public IndexBuffer
{
public:
	DynamicIndexBuffer();
	DynamicIndexBuffer(const uint *vertices, const uint indexCount);

	void modifyData(const uint startIdx, uint *indices, const uint indexCount);
};

class SAUCE_API StaticIndexBuffer : public IndexBuffer
{
public:
	StaticIndexBuffer();
	StaticIndexBuffer(const uint *vertices, const uint indexCount);
};

END_SAUCE_NAMESPACE
