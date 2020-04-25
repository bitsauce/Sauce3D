// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#pragma once

#include <Sauce/Common.h>

BEGIN_SAUCE_NAMESPACE

enum class TextAlignment
{
	Left,
	Centered,
	Right
};

class SAUCE_API FontRenderer
{
public:
	virtual ~FontRenderer() { }
	virtual bool initialize(const string& fontFilePath) = 0;
	virtual bool setFontSize(const uint32 fontSize) = 0;
	virtual void drawString(GraphicsContext* context,
		const string& str,
		const float x, const float y,
		const float scale=1.0f,
		const float rotation=0.0f,
		const TextAlignment alignment=TextAlignment::Left) = 0;
};

class SAUCE_API FontRenderingSystem
{
public:
	static bool initialize();
	static void free();
	static FontRenderer* createFontRenderer(const string& fontFilePath);
};

END_SAUCE_NAMESPACE
