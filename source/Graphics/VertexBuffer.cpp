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

/*********************************************************************
**	Vertex buffer													**
**********************************************************************/

VertexBuffer::VertexBuffer()
	: m_graphicsContext(nullptr)
	, m_deviceObject(nullptr)
{
}

VertexBuffer::~VertexBuffer()
{
	m_graphicsContext->vertexBuffer_destroyDeviceObject(m_deviceObject);
}

bool VertexBuffer::initialize(DescType vertexBufferDesc)
{
	// Get graphics context to use
	if (vertexBufferDesc.graphicsContext)
	{
		m_graphicsContext = vertexBufferDesc.graphicsContext;
	}
	else
	{
		m_graphicsContext = GraphicsContext::GetContext();
	}

	// Get debug name
	if (vertexBufferDesc.debugName.empty())
	{
		static uint32 anonymousVertexBufferCount = 0;
		vertexBufferDesc.debugName = "VertexBuffer_" + to_string(anonymousVertexBufferCount);
		anonymousVertexBufferCount++;
	}

	// Create and initialize vertex buffer device object
	m_graphicsContext->vertexBuffer_createDeviceObject(m_deviceObject, vertexBufferDesc.debugName);
	if (vertexBufferDesc.vertices)
	{
		m_graphicsContext->vertexBuffer_initializeVertexBuffer(
			m_deviceObject,
			vertexBufferDesc.bufferUsage,
			*vertexBufferDesc.vertices,
			vertexBufferDesc.vertexCount
		);
	}

	return true;
}

void VertexBuffer::modifyData(const uint32 startIndex, const VertexArray& vertices, const uint vertexCount)
{
	if (m_deviceObject->bufferUsage == BufferUsage::Static)
	{
		LOG("VertexBuffer::modifyData(): Cannot modify a static vertex buffer");
		return;
	}

	m_graphicsContext->vertexBuffer_modifyVertexBuffer(m_deviceObject, startIndex, vertices, vertexCount);
}

VertexFormat VertexBuffer::getVertexFormat() const
{
	return m_deviceObject->vertexFormat;
}

uint32 VertexBuffer::getVertexCount() const
{
	return m_deviceObject->vertexCount;
}

/*********************************************************************
**	Index buffer													**
**********************************************************************/

IndexBuffer::IndexBuffer()
	: m_graphicsContext(nullptr)
	, m_deviceObject(nullptr)
{
}

IndexBuffer::~IndexBuffer()
{
}

bool IndexBuffer::initialize(DescType indexBufferDesc)
{
	// Get graphics context to use
	if (indexBufferDesc.graphicsContext)
	{
		m_graphicsContext = indexBufferDesc.graphicsContext;
	}
	else
	{
		m_graphicsContext = GraphicsContext::GetContext();
	}

	// Get debug name
	if (indexBufferDesc.debugName.empty())
	{
		static uint32 anonymousIndexBufferCount = 0;
		indexBufferDesc.debugName = "IndexBuffer_" + to_string(anonymousIndexBufferCount);
		anonymousIndexBufferCount++;
	}

	// Create and initialize index buffer device object
	m_graphicsContext->indexBuffer_createDeviceObject(m_deviceObject, indexBufferDesc.debugName);
	m_graphicsContext->indexBuffer_initializeIndexBuffer(
		m_deviceObject,
		indexBufferDesc.bufferUsage,
		indexBufferDesc.indices,
		indexBufferDesc.indexCount
	);

	return true;
}

uint32 IndexBuffer::getIndexCount() const
{
	return m_deviceObject->indexCount;
}

END_SAUCE_NAMESPACE
