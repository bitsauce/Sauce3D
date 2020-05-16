// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#pragma once

#include <Sauce/Config.h>
#include <Sauce/Common/Window.h>

#include <SDL.h>
#include <SDL_syswm.h>

BEGIN_SAUCE_NAMESPACE

//--------------------------------------------------------------------
// SDL Window
//--------------------------------------------------------------------
class SDLWindow final : public Window
{
public:
	SDLWindow();
	~SDLWindow();

	virtual bool initialize(WindowDesc windowDesc) override;

	/**
	 * Fullscreen functionality
	 */
	void enableFullScreen(WindowFullscreenConfig* fullscreenConfig) override;
	vector<WindowFullscreenConfig> listFullscreenConfigs(const int32 displayIndex) const override;

	/**
	 * Windowed mode functionality
	 */
	void enableWindowed() override;
	void setWindowedPosition(const Vector2I& position) override;
	Vector2I getWindowedPosition() const override;
	void setWindowedSize(const Vector2I& size) override;
	Vector2I getWindowedMinimumSize() const override;
	void setWindowedMinimumSize(const Vector2I& minSize) override;
	Vector2I getWindowedMaximumSize() const override;
	void setWindowedMaximumSize(const Vector2I& maxSize) override;

	/**
	 * General window size
	 */
	Vector2I getSize() const override;

	/**
	 * Multi-screen functionality
	 */
	int32 getDisplayIndex() const override;

	/**
	 * Window titlebar functionality
	 */
	void setTitle(const string& title) override;
	string getTitle() const override;
	void setIcon(const Pixmap& icon) override;

	/**
	 * Window state functionality
	 */
	void show() override;
	void hide() override;
	void minimize() override;
	void restore() override;
	void maximize() override;

	/**
	 * Misc window functionality
	 */
	void setBordered(const bool bordered) override;
	void setMouseCapture(const bool captureMouse) override;
	void setBrightness(const float brightness) override;
	void setGammaRamp(uint16* red, uint16* green, uint16* blue) override;
	void setVSyncMode(const WindowVSyncMode vsyncMode) override;
	bool isFocused() const;
	void swapBuffers() const;
	void showMessageBox(MessageBoxType messageBoxType, const string& message) const;

	/** Window event handling */
	bool handleEvent(Game* game, SDL_Event* sdlEvent);
	int32 getWindowID() const override;
	void* getOSHandle() const override;
	SDL_Window* getSDLHandle() const;

private:
	SDL_Window*   m_window;
	SDL_GLContext m_glContext;
};

END_SAUCE_NAMESPACE
