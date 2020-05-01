// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#pragma once

// TODO:
// The following features are missing in the FontRenderer:
// - Should support boxed text rendering (e.g. re-implement FontRenderer::drawTextBox())
// - Should support vertical text rendering
// - Text coloring would be nice I suppose
// - 3D font placement

#include <Sauce/Common.h>
#include <Sauce/Utils.h>

BEGIN_SAUCE_NAMESPACE

/**
 * Text alignment enum
 */
enum class TextAlignment : uint32
{
	Left,
	Centered,
	Right
};

/**
 * FontRenderer descriptor object
 */
struct SAUCE_API FontRendererDesc : public SauceObjectDesc
{
	string fontFilePath;
	uint32 fontSize;

	FontRendererDesc()
		: fontFilePath("")
		, fontSize(128)
	{
	}

	/** Key that is used to look for cached font data files */
	string getKey() const
	{
		return util::FileMD5(fontFilePath) + "_" + to_string(fontSize);
	}
};

/** 
 * Struct used to control how FontRenderer::drawText() renders text
 */
struct SAUCE_API FontRendererDrawTextArgs
{
	string        text      = "";
	Vector2F      position  = Vector2F::Zero;
	float         scale     = 1.0f;
	float         rotation  = 0.0f;
	Matrix4       transform = Matrix4::Zero;
	TextAlignment alignment = TextAlignment::Left;
	float         edge0     = 0.4f;
	float         edge1     = 0.5f;
};

/**
 * Font rendering object
 */
class SAUCE_API FontRenderer : public SauceObject
{
public:
	SAUCE_REF_TYPE(FontRenderer);

	virtual ~FontRenderer() { }
	virtual bool initialize(FontRendererDesc objectDesc) = 0;
	virtual void drawText(GraphicsContext* context, FontRendererDrawTextArgs& args) = 0;

	/**
	 * Since FontRenderer is an abstract class, we define this static function
	 * to return a pointer to an instance of the implementation object. This
	 * allows us to create new FontRenderers via CreateNew<FontRenderer>()
	 */
	static FontRenderer* CreateImpl(FontRendererDesc desc);
};
SAUCE_REF_TYPE_TYPEDEFS(FontRenderer);

/**
 * Static class for initializing the font rendering system
 */
// TODO: Should remove this class if possible/makes sense
class SAUCE_API FontRenderingSystem
{
public:
	static bool Initialize(GraphicsContext* context);
	static void Free();
};

END_SAUCE_NAMESPACE
