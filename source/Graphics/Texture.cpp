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

Texture2D::Texture2D()
	: m_graphicsContext(nullptr)
	, m_deviceObject(nullptr)
{
}

Texture2D::~Texture2D()
{
}

bool Texture2D::initialize(DescType textureDesc)
{
	// Get graphics context to use
	if (textureDesc.graphicsContext)
	{
		m_graphicsContext = textureDesc.graphicsContext;
	}
	else
	{
		m_graphicsContext = GraphicsContext::GetContext();
	}

	// Get debug name
	if (textureDesc.debugName.empty())
	{
		static uint32 anonymousTexureCount = 0;
		textureDesc.debugName = "Texture_" + to_string(anonymousTexureCount);
		anonymousTexureCount++;
	}

	// Create texture device object
	m_graphicsContext->texture2D_createDeviceObject(m_deviceObject, textureDesc.debugName);

	// Set initial settings
	m_graphicsContext->texture2D_updateFiltering(m_deviceObject, textureDesc.filtering);
	m_graphicsContext->texture2D_updateWrapping(m_deviceObject, textureDesc.wrapping);

	// Set initial pixel data if a pixmap was provided
	if (!textureDesc.filePath.empty())
	{
		updatePixmap(Pixmap::loadFromFile(textureDesc.filePath));
	}
	else if (textureDesc.pixmap)
	{
		updatePixmap(*textureDesc.pixmap);
	}

	return true;
}

Pixmap Texture2D::getPixmap() const
{
	// Get texture data
	uchar* textureData;
	m_graphicsContext->texture2D_copyToCPUReadable(m_deviceObject, &textureData);

	// Copy data to pixmap
	Pixmap pixmap(m_deviceObject->width, m_deviceObject->height, m_deviceObject->pixelFormat, textureData);
	delete[] textureData;
	return pixmap;
}

void Texture2D::updatePixmap(const Pixmap& pixmap)
{
	m_graphicsContext->texture2D_copyToGPU(
		m_deviceObject,
		pixmap.getFormat(),
		pixmap.getWidth(),
		pixmap.getHeight(),
		(uint8*)pixmap.getData()
	);
}

void Texture2D::updatePixmap(const uint32 x, const uint32 y, const Pixmap& pixmap)
{
	if (x + pixmap.getWidth() >= m_deviceObject->width || y + pixmap.getHeight() >= m_deviceObject->height)
	{
		LOG("OpenGLContext::texture2D_updateSubregion(): Trying to update out-of-bounds texture data");
		return;
	}

	m_graphicsContext->texture2D_updateSubregion(
		m_deviceObject,
		x,
		y,
		pixmap.getWidth(),
		pixmap.getHeight(),
		(uint8*)pixmap.getData()
	);
}

void Texture2D::clear()
{
	m_graphicsContext->texture2D_clearTexture(m_deviceObject);
}

//void Texture2D::enableMipmaps()
//{
//	if(!m_mipmaps)
//	{
//		m_mipmaps = true;
//		updateFiltering();
//	}
//	else
//	{
//		//warn("Mipmapping already enabled");
//	}
//}
//
//void Texture2D::disableMipmaps()
//{
//	if(m_mipmaps)
//	{
//		m_mipmaps = false;
//		updateFiltering();
//	}
//	else
//	{
//		//warn("Mipmapping already disabled");
//	}
//}

void Texture2D::setFiltering(const TextureFiltering filtering)
{
	m_graphicsContext->texture2D_updateFiltering(m_deviceObject, filtering);
}

TextureFiltering Texture2D::getFiltering() const
{
	return m_deviceObject->filtering;
}

void Texture2D::setWrapping(const TextureWrapping wrapping)
{
	m_graphicsContext->texture2D_updateWrapping(m_deviceObject, wrapping);
}

TextureWrapping Texture2D::getWrapping() const
{
	return m_deviceObject->wrapping;
}

uint32 Texture2D::getWidth() const
{
	return m_deviceObject->width;
}

uint32 Texture2D::getHeight() const
{
	return m_deviceObject->height;
}

ByteStreamOut& operator<<(ByteStreamOut& out, const Texture2DRef& texture)
{
	const bool isNull = texture == nullptr;
	out << isNull;
	if (!isNull)
	{
		Pixmap pixmap = texture->getPixmap();
		out << pixmap;
		out << (uint32)texture->m_deviceObject->filtering;
		out << (uint32)texture->m_deviceObject->wrapping;
	}
	return out;
}

ByteStreamIn& operator>>(ByteStreamIn& in, Texture2DRef& texture)
{
	assert(texture == nullptr);

	bool isNull;
	in >> isNull;
	if (!isNull)
	{
		Texture2DDesc textureDesc;

		Pixmap pixmap;
		in >> pixmap;
		textureDesc.pixmap = &pixmap;

		in >> *(uint32*)&textureDesc.filtering;
		in >> *(uint32*)&textureDesc.wrapping;
		texture = CreateNew<Texture2D>(textureDesc);
	}
	return in;
}

END_SAUCE_NAMESPACE
