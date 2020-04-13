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
	: m_filter(TextureFiltering::NEAREST)
	, m_wrapping(TextureWrapping::CLAMP_TO_BORDER)
	, m_mipmaps(false)
	, m_mipmapsGenerated(false)
	, m_width(0)
	, m_height(0)
	, m_pixelFormat()
{
}

Texture2D::~Texture2D()
{
}

void Texture2D::enableMipmaps()
{
	if(!m_mipmaps)
	{
		m_mipmaps = true;
		updateFiltering();
	}
	else
	{
		//warn("Mipmapping already enabled");
	}
}

void Texture2D::disableMipmaps()
{
	if(m_mipmaps)
	{
		m_mipmaps = false;
		updateFiltering();
	}
	else
	{
		//warn("Mipmapping already disabled");
	}
}

void Texture2D::setFiltering(const TextureFiltering filter)
{
	if(m_filter != filter)
	{
		m_filter = filter;
		updateFiltering();
	}
	else
	{
		//warn("Texture filtering was not changed.");
	}
}

TextureFiltering Texture2D::getFiltering() const
{
	return m_filter;
}

void Texture2D::setWrapping(const TextureWrapping wrapping)
{
	if(m_wrapping != wrapping)
	{
		m_wrapping = wrapping;
		updateFiltering();
	}
}

TextureWrapping Texture2D::getWrapping() const
{
	return m_wrapping;
}

uint Texture2D::getWidth() const
{
	return m_width;
}

uint Texture2D::getHeight() const
{
	return m_height;
}

void Texture2D::exportToFile(string path)
{
	// TODO: This function might be redundant?
	getPixmap().saveToFile(path);
}

void *TextureResourceDesc::create() const
{
	// Load texture from file
	GraphicsContext *graphicsContext = Game::Get()->getWindow()->getGraphicsContext();
	Pixmap pixmap = Pixmap::loadFromFile(m_path);
	pixmap.setPremultipliedAlpha(m_premultiplyAlpha);
	return graphicsContext->createTexture(pixmap);
}

END_SAUCE_NAMESPACE
