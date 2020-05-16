// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#pragma once

#include <Sauce/Config.h>
#include <Sauce/Math.h>
#include <Sauce/Common/SauceObject.h>
#include <Sauce/Graphics/Pixmap.h>

BEGIN_SAUCE_NAMESPACE

SAUCE_FORWARD_DECLARE(GraphicsContext);

/*********************************************************************
**	Window class													**
**********************************************************************/

struct SAUCE_API WindowFullscreenConfig
{
	friend class Window;

private:
	int32  width;
	int32  height;
	int32  refreshRate;
	uint32 pixelFormat;
	uint32 displayIndex;
	uint32 displayModeIndex;

	WindowFullscreenConfig(const int32 width, const int32 height, const int32 refreshRate, const uint32 pixelFormat, const uint32 displayIndex, const uint32 displayModeIndex)
		: width(width)
		, height(height)
		, refreshRate(refreshRate)
		, pixelFormat(pixelFormat)
		, displayIndex(displayIndex)
		, displayModeIndex(displayModeIndex)
	{
	}

public:
	~WindowFullscreenConfig() { }
	int32 getWidth() { return width; }
	int32 getHeight() { return height; }
	int32  getRefreshRate() { return refreshRate; }
	uint32 getPixelFormat() { return pixelFormat; }
	uint32 getDisplayIndex() { return displayIndex; }
	uint32 getDisplayModeIndex() { return displayModeIndex; }
};

struct SAUCE_API WindowDesc : public SauceObjectDesc
{
	string title         = "Sauce3D";
	int32 initialScreenX = -1;
	int32 initialScreenY = -1;
	int32 width          = -1;
	int32 height         = -1;
	string iconFilePath  = "";
};

enum class WindowVSyncMode : uint32
{
	NoVSync,
	VSync,
	AdaptiveVSync
};

enum class MessageBoxType : uint32
{
	Error
};

/**
 * Base abstract window class
 */
class SAUCE_API Window : public SauceObject<Window, WindowDesc>
{
public:
	Window();
	~Window();

	virtual void postInitialize(RefType) override;

	/** Fullscreen functionality */
	virtual void enableFullScreen(WindowFullscreenConfig* fullscreenConfig=nullptr) = 0;
	virtual vector<WindowFullscreenConfig> listFullscreenConfigs(const int32 displayIndex=0) const = 0;

	/** Windowed mode functionality */
	virtual void enableWindowed() = 0;
	virtual void setWindowedPosition(const Vector2I& position) = 0;
	virtual Vector2I getWindowedPosition() const = 0;
	virtual void setWindowedSize(const Vector2I& size) = 0;
	virtual Vector2I getWindowedMinimumSize() const = 0;
	virtual void setWindowedMinimumSize(const Vector2I& minSize) = 0;
	virtual Vector2I getWindowedMaximumSize() const = 0;
	virtual void setWindowedMaximumSize(const Vector2I& maxSize) = 0;

	/** General window size */
	virtual Vector2I getSize() const = 0;

	/** Multi-screen functionality */
	virtual int32 getDisplayIndex() const = 0;

	/** Window titlebar functionality */
	virtual void setTitle(const string& title) = 0;
	virtual string getTitle() const = 0;
	virtual void setIcon(const Pixmap& iconPixmap) = 0;

	/** Window state functionality */
	virtual void hide() = 0;
	virtual void show() = 0;
	virtual void maximize() = 0;
	virtual void minimize() = 0;
	virtual void restore() = 0;

	/** Misc window functionality */
	virtual void setBordered(const bool bordered) = 0;
	virtual void setMouseCapture(const bool captureMouse) = 0;
	virtual void setBrightness(const float brightness) = 0;
	virtual void setGammaRamp(uint16* red, uint16* green, uint16* blue) = 0;
	virtual void setVSyncMode(const WindowVSyncMode vsyncMode) = 0;
	virtual bool isFocused() const = 0;
	virtual void swapBuffers() const = 0;
	virtual void showMessageBox(MessageBoxType messageBoxType, const string& message) const = 0;

	/** Misc getters */
	virtual void* getOSHandle() const = 0;
	virtual int32 getWindowID() const = 0;

	GraphicsContextRef getGraphicsContext() const;

	/**
	 * Convenience overloads
	 */
	void setWindowedPosition(const int32 x, const int32 y) { setWindowedPosition(Vector2I(x, y)); }
	void setWindowedPositionX(const int32 x) { setWindowedPosition(Vector2I(x, getWindowedPositionY())); }
	void setWindowedPositionY(const int32 y) { setWindowedPosition(Vector2I(getWindowedPositionX(), y)); }
	int32 getWindowedPositionX() const { return getWindowedPosition().x; }
	int32 getWindowedPositionY() const { return getWindowedPosition().y; }
	void setWindowedSize(const int32 x, const int32 y) { setWindowedSize(Vector2I(x, y)); }
	void setWindowedSizeX(const int32 x) { setWindowedSize(Vector2I(x, getHeight())); }
	void setWindowedSizeY(const int32 y) { setWindowedSize(Vector2I(getWidth(), y)); }
	int32 getWindowedMinimumSizeX() const { return getWindowedMinimumSize().x; }
	int32 getWindowedMinimumSizeY() const { return getWindowedMinimumSize().y; }
	void setWindowedMinimumSize(const int32 x, const int32 y) { setWindowedMinimumSize(Vector2I(x, y)); }
	void setWindowedMinimumSizeX(const int32 x) { setWindowedMinimumSize(Vector2I(x, getWindowedMinimumSizeY())); }
	void setWindowedMinimumSizeY(const int32 y) { setWindowedMinimumSize(Vector2I(getWindowedMinimumSizeX(), y)); }
	int32 getWindowedMaximumSizeX() const { return getWindowedMaximumSize().x; }
	int32 getWindowedMaximumSizeY() const { return getWindowedMaximumSize().y; }
	void setWindowedMaximumSize(const int32 x, const int32 y) { setWindowedMaximumSize(Vector2I(x, y)); }
	void setWindowedMaximumSizeX(const int32 x) { setWindowedMaximumSize(Vector2I(x, getWindowedMaximumSizeY())); }
	void setWindowedMaximumSizeY(const int32 y) { setWindowedMaximumSize(Vector2I(getWindowedMaximumSizeX(), y)); }
	int32 getWidth() const { return getSize().x; }
	int32 getHeight() const { return getSize().y; }

	static Window* CreateImpl();

protected:
	/** 
	 * Since we want to hide WindowFullscreenConfig's constructor,
	 * we provide this function for Window implementation classes
	 */
	static WindowFullscreenConfig CreateWindowFullscreenConfig(const int32 width, const int32 height, const int32 refreshRate, const uint32 pixelFormat, const uint32 displayIndex, const uint32 displayModeIndex);

	/** Graphics context object */
	GraphicsContextRef m_graphicsContext;
};
SAUCE_TYPEDEFS(Window);

END_SAUCE_NAMESPACE
