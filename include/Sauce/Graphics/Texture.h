// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#pragma once

#include <Sauce/Common.h>
#include <Sauce/Graphics/Pixmap.h>
#include <Sauce/Utils/FileSystemUtils.h>

BEGIN_SAUCE_NAMESPACE

// Texture filtering
enum class TextureFiltering : uint32
{
	NEAREST = GL_NEAREST,
	LINEAR = GL_LINEAR
};

// Texture wrapping
enum class TextureWrapping : uint32
{
	CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER,
	CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
	REPEAT = GL_REPEAT,
	MIRRORED_REPEAT = GL_MIRRORED_REPEAT
};

class SAUCE_API Texture2DDesc : public SauceObjectDesc {};

class SAUCE_API Texture2D : public SauceObject
{
	friend class RenderTarget2D;
	friend class GraphicsContext;
	friend class Shader;
public:
	SAUCE_REF_TYPE(Texture2D);

	Texture2D();
	virtual ~Texture2D();

	virtual bool initialize(Texture2DDesc) { return true; }

	// Mipmapping
	void enableMipmaps();
	void disableMipmaps();
	bool isMipmapsEnabled() const;

	void setFiltering(const TextureFiltering filter);
	TextureFiltering getFiltering() const;

	void setWrapping(const TextureWrapping wrapping);
	TextureWrapping getWrapping() const;

	// Size
	uint getWidth() const;
	uint getHeight() const;
	Vector2I getSize() const { return Vector2I(getWidth(), getHeight()); }

	// Texture data functions
	virtual Pixmap getPixmap() const = 0;
	virtual void updatePixmap(const Pixmap &pixmap) = 0;
	virtual void updatePixmap(const uint x, const uint y, const Pixmap &pixmap) = 0;
	virtual void clear() = 0;

	void exportToFile(string path);

	friend ByteStreamOut& operator<<(ByteStreamOut& out, const Texture2DRef& texture);
	friend ByteStreamIn& operator>>(ByteStreamIn& in, Texture2DRef& texture);

protected:
	virtual void updateFiltering() = 0;

	TextureFiltering m_filter;
	TextureWrapping m_wrapping;

	bool m_mipmaps;
	bool m_mipmapsGenerated;

	uint m_width;
	uint m_height;
	PixelFormat m_pixelFormat;
};
SAUCE_REF_TYPE_TYPEDEFS(Texture2D);

class TextureResourceDesc : public ResourceDesc
{
public:
	TextureResourceDesc(const string &name, const string &path, const bool premultiplyAlpha) :
		ResourceDesc(ResourceType::RESOURCE_TYPE_TEXTURE, name),
		m_premultiplyAlpha(premultiplyAlpha),
		m_path(path)
	{
	}

	void *create() const;

private:
	const bool m_premultiplyAlpha;
	const string m_path;
};

END_SAUCE_NAMESPACE

template class SAUCE_API std::shared_ptr<sauce::Texture2D>;
