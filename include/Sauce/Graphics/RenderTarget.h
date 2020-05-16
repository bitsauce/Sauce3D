// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#pragma once

#include <Sauce/Common.h>
#include <Sauce/Graphics/GraphicsDeviceObjectDesc.h>
#include <Sauce/Graphics/Texture.h>

BEGIN_SAUCE_NAMESPACE

SAUCE_FORWARD_DECLARE(GraphicsContext);

struct SAUCE_API RenderTarget2DDeviceObject
{
	virtual ~RenderTarget2DDeviceObject() { }

	uint32        targetCount    = 0;
	Texture2DRef* targetTextures = nullptr;
};

struct SAUCE_API RenderTarget2DDesc : public GraphicsDeviceObjectDesc
{
	uint32        width           = 0;
	uint32        height          = 0;
	uint32        targetCount     = 1;
	PixelFormat   pixelFormat     = PixelFormat(PixelComponents::Rgba, PixelDatatype::Uint8);
	Texture2DRef* targetTextures  = nullptr;
};

class SAUCE_API RenderTarget2D final : public SauceObject<RenderTarget2D, RenderTarget2DDesc>
{
	friend class GraphicsContext;

public:
	RenderTarget2D();
	virtual ~RenderTarget2D();

	bool initialize(DescType) override;

	Texture2DRef getTargetTexture(const uint32 targetIndex = 0) const;

private:
	GraphicsContextRef m_graphicsContext;
	RenderTarget2DDeviceObject* m_deviceObject;
};
SAUCE_TYPEDEFS(RenderTarget2D);

END_SAUCE_NAMESPACE
