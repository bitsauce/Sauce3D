//     _____                        ______             _            
//    / ____|                      |  ____|           (_)           
//   | (___   __ _ _   _  ___ ___  | |__   _ __   __ _ _ _ __   ___ 
//    \___ \ / _` | | | |/ __/ _ \ |  __| | '_ \ / _` | | '_ \ / _ \
//    ____) | (_| | |_| | (_|  __/ | |____| | | | (_| | | | | |  __/
//   |_____/ \__,_|\__,_|\___\___| |______|_| |_|\__, |_|_| |_|\___|
//                                                __/ |             
//                                               |___/              
// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#include <Sauce/Common.h>
#include <Sauce/Graphics.h>

BEGIN_SAUCE_NAMESPACE

VertexBuffer::VertexBuffer(const BufferUsage usage) :
	m_format(),
	m_usage(usage),
	m_size(0)
{
	glGenBuffers(1, &m_id);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &m_id);
}

void VertexBuffer::setData(const Vertex *vertices, const uint vertexCount)
{
	// Get vertex data
	m_format = vertices->getFormat();
	char *vertexData = new char[vertexCount * m_format.getVertexSizeInBytes()];
	for(uint i = 0; i < vertexCount; ++i)
	{
		vertices[i].getData(vertexData + i * m_format.getVertexSizeInBytes());
	}

	if(vertexCount > 0)
	{
		// Upload vertex data
		glBindBuffer(GL_ARRAY_BUFFER, m_id);
		glBufferData(GL_ARRAY_BUFFER, vertexCount * m_format.getVertexSizeInBytes(), vertexData, (uint32)m_usage);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	delete[] vertexData;

	m_size = vertexCount;
}

VertexFormat VertexBuffer::getVertexFormat() const
{
	return m_format;
}

DynamicVertexBuffer::DynamicVertexBuffer() :
	VertexBuffer(BufferUsage::Dynamic)
{
}

DynamicVertexBuffer::DynamicVertexBuffer(const Vertex *vertices, const uint vertexCount) :
	VertexBuffer(BufferUsage::Dynamic)
{
	setData(vertices, vertexCount);
}

void DynamicVertexBuffer::modifyData(const uint startIdx, Vertex *vertices, const uint vertexCount)
{
	if(!(m_format == vertices->getFormat())) return;

	// Get vertex data
	char *vertexData = new char[vertexCount * m_format.getVertexSizeInBytes()];
	for(uint i = 0; i < vertexCount; ++i)
	{
		vertices[i].getData(vertexData + i * m_format.getVertexSizeInBytes());
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_id);
	glBufferSubData(GL_ARRAY_BUFFER, startIdx * getVertexFormat().getVertexSizeInBytes(), vertexCount * getVertexFormat().getVertexSizeInBytes(), vertexData);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	delete[] vertexData;
}

StaticVertexBuffer::StaticVertexBuffer() :
	VertexBuffer(BufferUsage::Static)
{
}

StaticVertexBuffer::StaticVertexBuffer(const Vertex *vertices, const uint vertexCount) :
	VertexBuffer(BufferUsage::Static)
{
	setData(vertices, vertexCount);
}


// -------------------------------------------------------------------------------------

IndexBuffer::IndexBuffer(const BufferUsage usage)
	: m_usage(usage)
	, m_size(0)
{
	glGenBuffers(1, &m_id);
}

IndexBuffer::~IndexBuffer()
{
	glDeleteBuffers(1, &m_id);
}

void IndexBuffer::setData(const uint *indices, const uint indexCount)
{
	if(indexCount > 0)
	{
		// Upload index data
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(uint), indices, (uint32)m_usage);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	
	m_size = indexCount;
}

DynamicIndexBuffer::DynamicIndexBuffer() :
	IndexBuffer(BufferUsage::Dynamic)
{
}

DynamicIndexBuffer::DynamicIndexBuffer(const uint *indices, const uint indexCount) :
	IndexBuffer(BufferUsage::Dynamic)
{
	setData(indices, indexCount);
}

void DynamicIndexBuffer::modifyData(const uint startIdx, uint *indices, const uint indexCount)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, startIdx * sizeof(uint), indexCount * sizeof(uint), indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

StaticIndexBuffer::StaticIndexBuffer() :
	IndexBuffer(BufferUsage::Static)
{
}

StaticIndexBuffer::StaticIndexBuffer(const uint *indices, const uint indexCount) :
	IndexBuffer(BufferUsage::Static)
{
	setData(indices, indexCount);
}

END_SAUCE_NAMESPACE
