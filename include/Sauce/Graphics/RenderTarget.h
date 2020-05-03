// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#pragma once

#include <Sauce/Common.h>
#include <Sauce/Graphics/GraphicsContext.h>
#include <Sauce/Graphics/GraphicsDeviceObjectDesc.h>
#include <Sauce/Graphics/Texture.h>

BEGIN_SAUCE_NAMESPACE

struct SAUCE_API RenderTarget2DDeviceObject
{
	virtual ~RenderTarget2DDeviceObject() { }

	uint32        targetCount    = 0;
	Texture2DRef* targetTextures = nullptr;
};

struct SAUCE_API RenderTarget2DDesc : public GraphicsDeviceObjectDesc
{
	uint32           width           = 0;
	uint32           height          = 0;
	uint32           targetCount     = 1;
	PixelFormat      pixelFormat     = PixelFormat(PixelComponents::RGBA, PixelDatatype::UNSIGNED_BYTE);
	Texture2DRef*    targetTextures  = nullptr;
};

class SAUCE_API RenderTarget2D final : public SauceObject
{
	friend class GraphicsContext;
public:
	SAUCE_REF_TYPE(RenderTarget2D);

	RenderTarget2D();
	virtual ~RenderTarget2D();

	bool initialize(RenderTarget2DDesc renderTargetDesc);

	Texture2DRef getTargetTexture(const uint32 targetIndex = 0) const;

private:
	GraphicsContext* m_graphicsContext;
	RenderTarget2DDeviceObject* m_deviceObject;
};
SAUCE_REF_TYPE_TYPEDEFS(RenderTarget2D);

END_SAUCE_NAMESPACE
