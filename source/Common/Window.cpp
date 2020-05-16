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

// Public headers
#include <Sauce/Common.h>
#include <Sauce/Graphics.h>

// Private headers
#include <Common/SDL/SDLWindow.h>

BEGIN_SAUCE_NAMESPACE

//--------------------------------------------------------------------
// Window
//--------------------------------------------------------------------

Window::Window()
	: m_graphicsContext(nullptr)
{
}

Window::~Window()
{
}

bool Window::initialize(WindowDesc windowDesc)
{
	m_this = WindowRef(this);

	// Create a graphics context
	GraphicsContextDesc graphicsContextDesc;
	graphicsContextDesc.owningWindow = m_this;
	m_graphicsContext = CreateNew<GraphicsContext>(graphicsContextDesc);
	return m_graphicsContext != nullptr;
}

GraphicsContextRef Window::getGraphicsContext() const
{
	return m_graphicsContext;
}

Window* Window::CreateImpl()
{
	return new SDLWindow();
}

WindowFullscreenConfig Window::CreateWindowFullscreenConfig(
	const int32 width, const int32 height, const int32 refreshRate,
	const uint32 pixelFormat, const uint32 displayIndex, const uint32 displayModeIndex)
{
	return WindowFullscreenConfig(width, height, refreshRate, pixelFormat, displayIndex, displayModeIndex);
}

END_SAUCE_NAMESPACE

