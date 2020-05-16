// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#pragma once

#include <Sauce/Common.h>
#include <Sauce/Graphics/Pixmap.h>
#include <Sauce/Graphics/GraphicsDeviceObjectDesc.h>
#include <Sauce/Utils/FileSystemUtils.h>

BEGIN_SAUCE_NAMESPACE

SAUCE_FORWARD_DECLARE(GraphicsContext);

enum class TextureFiltering : uint32
{
	Nearest,
	Linear
};

enum class TextureWrapping : uint32
{
	ClampToBorder,
	ClampToEdge,
	Repeat,
	MirroredRepeat
};

struct SAUCE_API Texture2DDeviceObject
{
	virtual ~Texture2DDeviceObject() { }

	uint32           width       = 0;
	uint32           height      = 0;
	TextureFiltering filtering   = TextureFiltering::Nearest;
	TextureWrapping  wrapping    = TextureWrapping::ClampToBorder;
	PixelFormat      pixelFormat = PixelFormat();
	bool             hasMipmaps  = false;
};

struct SAUCE_API Texture2DDesc : public GraphicsDeviceObjectDesc
{
	string           filePath  = "";
	Pixmap*          pixmap    = nullptr;
	TextureFiltering filtering = TextureFiltering::Nearest;
	TextureWrapping  wrapping  = TextureWrapping::ClampToEdge;
	bool             mipmaps   = false;
};

class SAUCE_API Texture2D final : public SauceObject
{
	friend class RenderTarget2D;
	friend class GraphicsContext;
	friend class Shader;
public:
	SAUCE_REF_TYPE(Texture2D);

	Texture2D();
	~Texture2D();

	bool initialize(Texture2DDesc textureDesc);

	// Mipmapping
	//void enableMipmaps();
	//void disableMipmaps();
	//bool isMipmapsEnabled() const;

	void setFiltering(const TextureFiltering filtering);
	TextureFiltering getFiltering() const;

	void setWrapping(const TextureWrapping wrapping);
	TextureWrapping getWrapping() const;

	// Size
	uint32 getWidth() const;
	uint32 getHeight() const;
	Vector2I getSize() const { return Vector2I(getWidth(), getHeight()); }

	// Texture data functions
	Pixmap getPixmap() const;
	void updatePixmap(const Pixmap &pixmap);
	void updatePixmap(const uint32 x, const uint32 y, const Pixmap &pixmap);
	void clear();

	friend ByteStreamOut& operator<<(ByteStreamOut& out, const Texture2DRef& texture);
	friend ByteStreamIn& operator>>(ByteStreamIn& in, Texture2DRef& texture);

private:
	GraphicsContextRef m_graphicsContext;
	Texture2DDeviceObject* m_deviceObject;
};
SAUCE_REF_TYPE_TYPEDEFS(Texture2D);

END_SAUCE_NAMESPACE
