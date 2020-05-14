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

RenderTarget2D::RenderTarget2D() 
	: m_graphicsContext(nullptr)
	, m_deviceObject(nullptr)
{
}

RenderTarget2D::~RenderTarget2D()
{
	m_graphicsContext->renderTarget2D_destroyDeviceObject(m_deviceObject);
}

bool RenderTarget2D::initialize(RenderTarget2DDesc renderTargetDesc)
{
	if (renderTargetDesc.width == 0 || renderTargetDesc.height == 0)
	{
		LOG("CreateNew<RenderTarget2D>(): width and height must be greater than 0.");
		return false;
	}

	// Get graphics context to use
	if (renderTargetDesc.graphicsContext)
	{
		m_graphicsContext = renderTargetDesc.graphicsContext;
	}
	else
	{
		m_graphicsContext = GraphicsContext::GetContext();
	}

	// Get debug name
	if (renderTargetDesc.debugName.empty())
	{
		static uint32 anonymousRenderTargetCount = 0;
		renderTargetDesc.debugName = "RenderTarget2D_" + to_string(anonymousRenderTargetCount);
		anonymousRenderTargetCount++;
	}

	// Create device object
	m_graphicsContext->renderTarget2D_createDeviceObject(m_deviceObject, renderTargetDesc.debugName);

	// Get target textures
	Texture2DRef* targetTextures;
	if (renderTargetDesc.targetTextures == nullptr)
	{
		// Allocate target textures
		targetTextures = new Texture2DRef[renderTargetDesc.targetCount];
		for (uint32 i = 0; i < renderTargetDesc.targetCount; ++i)
		{
			Texture2DDesc textureDesc;
			Pixmap pixmap(renderTargetDesc.width, renderTargetDesc.height, renderTargetDesc.pixelFormat);
			textureDesc.pixmap = &pixmap;
			targetTextures[i] = CreateNew<Texture2D>(textureDesc);
		}
	}
	else
	{
		targetTextures = renderTargetDesc.targetTextures;
	}

	// Initialize render target with target textures
	m_graphicsContext->renderTarget2D_initializeRenderTarget(m_deviceObject, targetTextures, renderTargetDesc.targetCount);

	// Clean up allocated memory
	if (renderTargetDesc.targetTextures == nullptr)
	{
		delete[] targetTextures;
	}

	return true;
}

Texture2DRef RenderTarget2D::getTargetTexture(const uint32 targetIndex) const
{
	if (targetIndex < m_deviceObject->targetCount)
	{
		return m_deviceObject->targetTextures[targetIndex];
	}
	return nullptr;
}

END_SAUCE_NAMESPACE
