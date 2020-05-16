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

#include <Common/SDL/SDLWindow.h>
#include <Sauce/Graphics/GraphicsContext.h>

BEGIN_SAUCE_NAMESPACE

SDLWindow::SDLWindow()
	: m_window(nullptr)
	, m_glContext(nullptr)
{
}

SDLWindow::~SDLWindow()
{
	if (GraphicsContext::GetEffectiveBackend() == GraphicsBackend::OpenGL4)
	{
		SDL_GL_DeleteContext(m_glContext);
	}
	SDL_DestroyWindow(m_window);
}

bool SDLWindow::initialize(WindowDesc windowDesc)
{
	if (windowDesc.initialScreenX < 0)
	{
		windowDesc.initialScreenX = SDL_WINDOWPOS_CENTERED;
	}

	if (windowDesc.initialScreenY < 0)
	{
		windowDesc.initialScreenY = SDL_WINDOWPOS_CENTERED;
	}

	if (windowDesc.width < 0)
	{
		SDL_DisplayMode displayMode;
		SDL_GetCurrentDisplayMode(0, &displayMode);
		windowDesc.width = int32(displayMode.w / 1.5f);
	}

	if (windowDesc.height < 0)
	{
		SDL_DisplayMode displayMode;
		SDL_GetCurrentDisplayMode(0, &displayMode);
		windowDesc.height = int32(displayMode.h / 1.5f);
	}

	// OpenGL graphics backend need a special SDL flag
	Uint32 windowFlags = 0;
	if (GraphicsContext::GetEffectiveBackend() == GraphicsBackend::OpenGL4)
	{
		windowFlags |= SDL_WINDOW_OPENGL;

		// Request OpenGL context
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

		// Turn on double buffering with a 24bit Z buffer.
		// You may need to change this to 16 or 32 for your system
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	}

	if (Game::Get()->isEnabled(EngineFlag::ResizableWindow))
	{
		windowFlags |= SDL_WINDOW_RESIZABLE;
	}

	// Create the SDL window handle
	m_window = SDL_CreateWindow(
		windowDesc.title.c_str(),
		windowDesc.initialScreenX,
		windowDesc.initialScreenY,
		windowDesc.width,
		windowDesc.height,
		windowFlags);

	if (!m_window)
	{
		THROW("SDL failed to create the game window");
		return false;
	}

	// TODO: iconFilePath

	// Create an SDL graphics context handle
	if (GraphicsContext::GetEffectiveBackend() == GraphicsBackend::OpenGL4)
	{
		m_glContext = SDL_GL_CreateContext(m_window);
		SDL_GL_MakeCurrent(m_window, m_glContext);
	}

	// Initialize the engine rendering context
	if (!Window::initialize(windowDesc))
	{
		return false;
	}

	return true;
}

/**
* Fullscreen functionality
*/

void SDLWindow::enableFullScreen(WindowFullscreenConfig* fullscreenConfig)
{
	SDL_DisplayMode displayMode;
	if (fullscreenConfig)
	{
		SDL_GetDisplayMode(fullscreenConfig->getDisplayIndex(), fullscreenConfig->getDisplayModeIndex(), &displayMode);
	}
	else
	{
		// Use the display mode of the primary display if no fullscreen config was provided
		SDL_GetDesktopDisplayMode(0, &displayMode);
	}

	// Set display mode and enable the fullscreen flag
	SDL_SetWindowDisplayMode(m_window, &displayMode);
	SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN);
}

vector<WindowFullscreenConfig> SDLWindow::listFullscreenConfigs(const int32 displayIndex) const
{
	vector<WindowFullscreenConfig> configs;
	const int32 numDisplayModes = SDL_GetNumDisplayModes(displayIndex);
	for (int32 displayModeIndex = 0; displayModeIndex < numDisplayModes; ++displayModeIndex)
	{
		SDL_DisplayMode displayMode;
		SDL_GetDisplayMode(displayIndex, displayModeIndex, &displayMode);
		configs.push_back(Window::CreateWindowFullscreenConfig(displayMode.w, displayMode.h, displayMode.refresh_rate, displayMode.format, displayIndex, displayModeIndex));
	}
	return configs;
}

/**
* Windowed mode functionality
*/

void SDLWindow::enableWindowed()
{
	SDL_SetWindowFullscreen(m_window, 0);
}

void SDLWindow::setWindowedPosition(const Vector2I& position)
{
	SDL_SetWindowPosition(m_window, position.x, position.y);
}

Vector2I SDLWindow::getWindowedPosition() const
{
	Vector2I pos;
	SDL_GetWindowPosition(m_window, &pos.x, &pos.y);
	return pos;
}

void SDLWindow::setWindowedSize(const Vector2I& size)
{
	SDL_SetWindowSize(m_window, size.x, size.y);
}

Vector2I SDLWindow::getWindowedMinimumSize() const
{
	Vector2I size;
	SDL_GetWindowMinimumSize(m_window, &size.x, &size.y);
	return size;
}

void SDLWindow::setWindowedMinimumSize(const Vector2I& minSize)
{
	SDL_SetWindowMinimumSize(m_window, minSize.x, minSize.y);
}

Vector2I SDLWindow::getWindowedMaximumSize() const
{
	Vector2I size;
	SDL_GetWindowMaximumSize(m_window, &size.x, &size.y);
	return size;
}

void SDLWindow::setWindowedMaximumSize(const Vector2I& maxSize)
{
	SDL_SetWindowMaximumSize(m_window, maxSize.x, maxSize.y);
}

/**
* General window size
*/

Vector2I SDLWindow::getSize() const
{
	Vector2I size;
	SDL_GetWindowSize(m_window, &size.x, &size.y);
	return size;
}

/**
* Multi-screen functionality
*/

int32 SDLWindow::getDisplayIndex() const
{
	return SDL_GetWindowDisplayIndex(m_window);
}

/**
* Window titlebar functionality
*/

void SDLWindow::setTitle(const string& title)
{
	SDL_SetWindowTitle(m_window, title.c_str());
}

string SDLWindow::getTitle() const
{
	return SDL_GetWindowTitle(m_window);
}

void SDLWindow::setIcon(const Pixmap& icon)
{
	Uint32 rmask, gmask, bmask, amask;

	// SDL interprets each pixel as a 32-bit number, so our masks must depend
	// on the endianness (byte order) of the machine
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif

	SDL_Surface* surface = SDL_CreateRGBSurfaceFrom((void*)icon.getData(), icon.getWidth(), icon.getHeight(), 32, icon.getWidth() * 4, rmask, gmask, bmask, amask);
	SDL_SetWindowIcon(m_window, surface);
	SDL_FreeSurface(surface);
}

/**
* Window state functionality
*/

void SDLWindow::show()
{
	SDL_ShowWindow(m_window);
}

void SDLWindow::hide()
{
	SDL_HideWindow(m_window);
}

void SDLWindow::minimize()
{
	SDL_MinimizeWindow(m_window);
}

void SDLWindow::restore()
{
	SDL_RestoreWindow(m_window);
}

void SDLWindow::maximize()
{
	SDL_MaximizeWindow(m_window);
}

/**
* Misc window functionality
*/

void SDLWindow::setBordered(const bool bordered)
{
	SDL_SetWindowBordered(m_window, bordered ? SDL_TRUE : SDL_FALSE);
}

void SDLWindow::setMouseCapture(const bool captureMouse)
{
	SDL_SetWindowGrab(m_window, captureMouse ? SDL_TRUE : SDL_FALSE);
}

void SDLWindow::setBrightness(const float brightness)
{
	SDL_SetWindowBrightness(m_window, brightness);
}

void SDLWindow::setGammaRamp(uint16* red, uint16* green, uint16* blue)
{
	SDL_SetWindowGammaRamp(m_window, red, green, blue);
}

void SDLWindow::setVSyncMode(const WindowVSyncMode vsyncMode)
{
	int32 vsyncModeSDL;
	switch (vsyncMode)
	{
		case WindowVSyncMode::VSync:            vsyncModeSDL = 1; break;
		case WindowVSyncMode::AdaptiveVSync:    vsyncModeSDL = -1; break;
		case WindowVSyncMode::NoVSync: default: vsyncModeSDL = 0;
	}

	if (GraphicsContext::GetEffectiveBackend() == GraphicsBackend::OpenGL4)
	{
		SDL_GL_SetSwapInterval(vsyncModeSDL);
	}
}

bool SDLWindow::isFocused() const
{
	return (SDL_GetWindowFlags(m_window) & SDL_WINDOW_INPUT_FOCUS) != 0;
}

void SDLWindow::swapBuffers() const
{
	SDL_GL_SwapWindow(m_window);
}

void SDLWindow::showMessageBox(MessageBoxType messageBoxType, const string& message) const
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "An error occured", message.c_str(), m_window);
}

bool SDLWindow::handleEvent(Game* game, SDL_Event* sdlEvent)
{
	switch (sdlEvent->window.event)
	{
		case SDL_WINDOWEVENT_CLOSE:
		{
			return true;
		}
		break;

		case SDL_WINDOWEVENT_SIZE_CHANGED:
		{
			// Resize viewport
			int width = sdlEvent->window.data1, height = sdlEvent->window.data2;
			m_graphicsContext->setSize(width, height);

			// Send a window size changed event
			{
				WindowEvent windowEvent(WindowEventType::SizeChanged, this, width, height);
				game->onEvent(&windowEvent);
			}

			// Send a mouse move event
			{
				Vector2I position = game->getInputManager()->getPosition();
				MouseEvent mouseEvent(MouseEventType::Move, game->getInputManager(), position.x, position.y, SAUCE_MOUSE_BUTTON_NONE, 0, 0);
				game->onEvent(&mouseEvent);
			}
		}
		break;
	}
	return false;
}

void* SDLWindow::getOSHandle() const
{
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	assert(SDL_GetWindowWMInfo(m_window, &info));
	return info.info.win.window;
}

SDL_Window* SDLWindow::getSDLHandle() const
{
	return m_window;
}

int32 SDLWindow::getWindowID() const
{
	return SDL_GetWindowID(m_window);
}

END_SAUCE_NAMESPACE
