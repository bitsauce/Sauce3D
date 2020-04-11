//--------------------------------------------------------------
// SauceEngine
// Made by Marcus "Bitsauce" Loo Vergara
// 2011-2018 (C)
//--------------------------------------------------------------

#pragma once

#include <Sauce/Common.h>

BEGIN_SAUCE_NAMESPACE

class SAUCE_API FontRenderer
{
public:
	virtual ~FontRenderer() { }
	virtual bool initialize(const string& fontFilePath) = 0;
	virtual bool setFontSize(const uint32 fontSize) = 0;
	virtual void drawString(GraphicsContext* context, const string& str) = 0;
};

class SAUCE_API FontRenderingSystem
{
public:
	static bool initialize();
	static void free();
	static FontRenderer* createFontRenderer(const string& fontFilePath);
};

END_SAUCE_NAMESPACE
