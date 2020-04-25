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

// Public headers
#include <Sauce/Common.h>
#include <Sauce/Graphics.h>
#include <Sauce/Input.h>
#include <Sauce/Audio.h>

// Private headers
#include <ImGui/ImGuiSystem.h>

// Additional headers
#include <FreeImage.h>
#undef WIN32_LEAN_AND_MEAN // "warning C4005: macro redefinition" fix
#include <SDL_syswm.h>

BEGIN_SAUCE_NAMESPACE

Keycode KeyEvent::getKeycode() const
{
	return (Keycode) SDL_GetKeyFromScancode((SDL_Scancode) m_inputButton.getCode());
}

/**
 * FreeImage error handler
 * @param fif Format / Plugin responsible for the error
 * @param message Error message
 */
void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message)
{
	LOG("\n*** ");
	if(fif != FIF_UNKNOWN) {
		LOG("%s Format\n", FreeImage_GetFormatFromFIF(fif));
	}
	LOG(message);
	LOG(" ***\n");
}

Game *Game::s_this = 0;

Game::Game()
	: m_initialized(false)
	, m_paused(false)
	, m_running(false)
	, m_console(nullptr)
	, m_fileSystem(nullptr)
	, m_framesPerSecond(0.0)
	, m_inputManager(nullptr)
	, m_resourceManager(nullptr)
	, m_scene(nullptr)
	, m_timer(nullptr)
{
	THROW_IF(s_this != nullptr, "Game instance already exists!");
	s_this = this;
}

Game::~Game()
{
	// Release managers
	delete m_fileSystem;
	delete m_timer;
	delete m_console;
	delete m_resourceManager;
	s_this = 0;
}

//------------------------------------------------------------------------
// Run
//------------------------------------------------------------------------
int Game::run(const GameDesc &desc)
{
	try
	{
		m_desc = desc;

		// Set cwd
		SetCurrentDirectory(desc.workingDirectory.c_str());

		// Make sure we're not running already
		THROW_IF(m_running, "Game is already running");
		m_running = true;

		// TODO: Implement an engine config file
		// (DefaultConfig.ini for instance). It should
		// set the value of SAUCE_EXPORT_LOG and SAUCE_VERBOSE, etc.
		//
		// ConfigFile default("config:/DefaultConfig.ini");
		// default.getValue("Window/ResolutionX");
		// etc...

#ifdef SAUCE_COMPILE_WINDOWS
		for(int i = 0; i < __argc; i++)
		{
			string argType = __argv[i];
			if(argType == "-cwd")
			{
				string arg = __argv[++i];
				SetCurrentDirectory(arg.c_str());
			}
		}
#endif

		// Set game root directory
		m_binaryPath = SDL_GetBasePath();

		// Set save directory
		m_prefPath = SDL_GetPrefPath(desc.organization.c_str(), desc.name.c_str());

		m_console = new Console();
		//m_fileSystem = new FileSystem();
		if(isEnabled(EngineFlag::SAUCE_EXPORT_LOG))
		{
			m_console->m_output = new ofstream();
			m_console->m_output->open("console.log");
		}

		m_timer = new Timer();
		//m_audio = new AudioManager();

		m_console->m_engine = this;

		LOG("** Initializing Engine **");

		// Initialize SDL
		THROW_IF(SDL_Init(SDL_INIT_EVERYTHING) < 0, "Unable to initialize SDL");

		SDL_version sdlver;
		SDL_GetVersion(&sdlver);
		LOG("** SDL %i.%i.%i initialized **", sdlver.major, sdlver.minor, sdlver.patch);

		// Set FreeImage message callback
		FreeImage_SetOutputMessage(FreeImageErrorHandler);

		// Initialize font rendering system
		FontRenderingSystem::initialize();

		// Initialize resource manager
		m_resourceManager = new ResourceManager("Resources.xml");

		Uint32 windowFlags = 0;
		if(isEnabled(EngineFlag::SAUCE_WINDOW_RESIZABLE))
		{
			windowFlags |= SDL_WINDOW_RESIZABLE;
		}

		// Initialize graphics context and window
		GraphicsContext *graphicsContext = 0;
		switch(desc.graphicsBackend)
		{
			default: graphicsContext = new OpenGLContext(4, 2); break;
		}
		Window *mainWindow = graphicsContext->createWindow(desc.name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, windowFlags);
		m_windows.push_back(mainWindow);

		// Setup default vertex format
		VertexFormat::s_vct.set(VertexAttribute::VERTEX_POSITION, 2, Datatype::SAUCE_FLOAT);
		VertexFormat::s_vct.set(VertexAttribute::VERTEX_COLOR, 4, Datatype::SAUCE_UBYTE);
		VertexFormat::s_vct.set(VertexAttribute::VERTEX_TEX_COORD, 2, Datatype::SAUCE_FLOAT);

		// Initialize input handler
		m_inputManager = new InputManager("InputConfig.xml");

		m_scene = new Scene(this);
		
		// Start listening for SDL text input
		SDL_StartTextInput();

		// Initialize ImGui
		{
			SDL_SysWMinfo info;
			SDL_VERSION(&info.version);
			assert(SDL_GetWindowWMInfo(mainWindow->getSDLHandle(), &info));
			ImGuiSystem::initialize(info.info.win.window);
		}

		// Engine initialized
		m_initialized = true;

		LOG("** Engine Initialized **");

		// Call onStart event
		{
			GameEvent e(GameEventType::START);
			onEvent(&e);
		}
		
		// Fps sampling
		const uint numFpsSamples = 8;
		double fpsSamples[numFpsSamples];
		for(uint i = 0; i < numFpsSamples; ++i) fpsSamples[i] = 0.0;
		uint currFpsSample = 0;

		// Setup game loop
		m_timer->start();
		const double dt = desc.deltaTime;
		double accumulator = 0.0;
		double prevTime = m_timer->getElapsedTime();

		// Make sure update is called once before draw
		{
			TickEvent e(dt);
			onEvent(&e);
		}

		// Game loop
		while(m_running)
		{
			// Event handling
			SDL_Event event;
			char textInputChar = '\0';
			while(SDL_PollEvent(&event))
			{
				switch(event.type)
				{
					case SDL_WINDOWEVENT:
					{
						list<Window*> windows(m_windows);
						for(Window *window : windows)
						{
							if(event.window.windowID == window->getID())
							{
								if(window->handleEvent(event, this))
								{
									// The window was closed. Release its resources.
									m_windows.remove(window);
									delete window;

									// If all windows are closed, end the game.
									if(m_windows.size() == 0)
									{
										end();
										goto gameloopend;
									}
								}
							}
						}
					}
					break;

					case SDL_KEYUP: case SDL_KEYDOWN:
					{
						// Send key input event
						KeyEvent e(
							event.type == SDL_KEYDOWN ?
							(event.key.repeat == 0 ? KeyEventType::DOWN : KeyEventType::REPEAT) :
							KeyEventType::UP,
							m_inputManager,
							(Scancode)event.key.keysym.scancode,
							event.key.keysym.mod);
						onEvent(&e);
						m_inputManager->updateKeybinds(&e);
					}
					break;

					case SDL_TEXTINPUT:
					{
						// If no modifiers are pressed
						if((SDL_GetModState() & (KMOD_CTRL | KMOD_ALT)) == 0)
						{
							// Send text input event
							TextEvent e(event.text.text[0]);
							onEvent(&e);
						}
						textInputChar = event.text.text[0];
					}
					break;

					case SDL_MOUSEMOTION:
					{
						// Update mouse position
						m_inputManager->m_x = event.motion.x;
						m_inputManager->m_y = event.motion.y;

						// Send mouse move event
						MouseEvent e(MouseEventType::MOVE, m_inputManager, event.motion.x, event.motion.y, SAUCE_MOUSE_BUTTON_NONE, 0, 0);
						onEvent(&e);
					}
					break;

					case SDL_MOUSEBUTTONDOWN:
					{
						// MouseEvent
						MouseEvent mouseEvent(MouseEventType::DOWN, m_inputManager, m_inputManager->m_x, m_inputManager->m_y, (const MouseButton) event.button.button, 0, 0);
						onEvent(&mouseEvent);

						// KeyEvent
						KeyEvent keyEvent(KeyEventType::DOWN, m_inputManager, (const MouseButton) event.button.button, event.key.keysym.mod);
						onEvent(&keyEvent);
						m_inputManager->updateKeybinds(&keyEvent);
					}
					break;

					case SDL_MOUSEBUTTONUP:
					{
						// MouseEvent
						MouseEvent mouseEvent(MouseEventType::UP, m_inputManager, m_inputManager->m_x, m_inputManager->m_y, (const MouseButton) event.button.button, 0, 0);
						onEvent(&mouseEvent);

						// KeyEvent
						KeyEvent keyEvent(KeyEventType::UP, m_inputManager, (const MouseButton) event.button.button, event.key.keysym.mod);
						onEvent(&keyEvent);
						m_inputManager->updateKeybinds(&keyEvent);
					}
					break;

					case SDL_MOUSEWHEEL:
					{
						// Scroll event
						MouseEvent mouseEvent(MouseEventType::WHEEL, m_inputManager, m_inputManager->m_x, m_inputManager->m_y, SAUCE_MOUSE_BUTTON_NONE, event.wheel.x, event.wheel.y);
						onEvent(&mouseEvent);
					}
					break;

					case SDL_CONTROLLERDEVICEADDED:
					{
						m_inputManager->addController(event.cdevice.which);
						//ControllerDeviceEvent e();
						//onEvent(&e);
					}
					break;

					case SDL_CONTROLLERDEVICEREMOVED:
					{
						m_inputManager->removeController(event.cdevice.which);
						//ControllerDeviceEvent e();
						//onEvent(&e);
					}
					break;

					case SDL_CONTROLLERBUTTONDOWN:
					{
						// Send controller button event
						ControllerButtonEvent e(ControllerButtonEventType::DOWN, m_inputManager, (const ControllerButton)event.cbutton.button);// , event.cbutton.which);
						onEvent(&e);
						m_inputManager->updateKeybinds(&e);
					}
					break;

					case SDL_CONTROLLERBUTTONUP:
					{
						// Send controller button event
						ControllerButtonEvent e(ControllerButtonEventType::UP, m_inputManager, (const ControllerButton) event.cbutton.button);// , event.cbutton.which);
						onEvent(&e);
						m_inputManager->updateKeybinds(&e);
					}
					break;
  
					case SDL_CONTROLLERAXISMOTION:
					{
						// If the axis is a trigger button
						if(event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT)
						{
							if(!m_inputManager->getButtonState(SAUCE_CONTROLLER_BUTTON_RIGHT_TRIGGER))
							{
								// And the axis exceedes the threshold value
								if(AXIS_VALUE_TO_FLOAT(event.caxis.value) >= m_inputManager->m_triggerThreshold)
								{
									// Flag trigger as pressed and send controller button event
									m_inputManager->m_rightTrigger = true;
									ControllerButtonEvent e(ControllerButtonEventType::DOWN, m_inputManager, SAUCE_CONTROLLER_BUTTON_RIGHT_TRIGGER);// , event.cbutton.which);
									onEvent(&e);
									m_inputManager->updateKeybinds(&e);
								}
							}
							else
							{
								if(AXIS_VALUE_TO_FLOAT(event.caxis.value) < m_inputManager->m_triggerThreshold)
								{
									m_inputManager->m_rightTrigger = false;
									ControllerButtonEvent e(ControllerButtonEventType::UP, m_inputManager, SAUCE_CONTROLLER_BUTTON_RIGHT_TRIGGER);// , event.cbutton.which);
									onEvent(&e);
									m_inputManager->updateKeybinds(&e);
								}
							}
						}
						else if(event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT)
						{
							if(!m_inputManager->getButtonState(SAUCE_CONTROLLER_BUTTON_LEFT_TRIGGER))
							{
								if(event.caxis.value >= m_inputManager->m_triggerThreshold)
								{
									m_inputManager->m_leftTrigger = true;
									ControllerButtonEvent e(ControllerButtonEventType::DOWN, m_inputManager, SAUCE_CONTROLLER_BUTTON_LEFT_TRIGGER);// , event.cbutton.which);
									onEvent(&e);
									m_inputManager->updateKeybinds(&e);
								}
							}
							else
							{
								if(event.caxis.value < m_inputManager->m_triggerThreshold)
								{
									m_inputManager->m_leftTrigger = false;
									ControllerButtonEvent e(ControllerButtonEventType::UP, m_inputManager, SAUCE_CONTROLLER_BUTTON_LEFT_TRIGGER);// , event.cbutton.which);
									onEvent(&e);
									m_inputManager->updateKeybinds(&e);
								}
							}
						}

						// Send controller axis event
						ControllerAxisEvent e(m_inputManager, (const ControllerAxis) event.caxis.axis, event.caxis.value);// , event.cbutton.which);
						onEvent(&e);
						m_inputManager->updateKeybinds(&e);
					}
					break;
				}
			}

			// Check if game is paused or out of focus
			if(m_paused || (!isEnabled(EngineFlag::SAUCE_RUN_IN_BACKGROUND) && !mainWindow->checkFlags(SDL_WINDOW_INPUT_FOCUS)))
			{
				continue;
			}

			// Calculate time delta
			const double currentTime = m_timer->getElapsedTime();
			double deltaTime = currentTime - prevTime;
			prevTime = currentTime;

			// Avoid spiral of death
			if(deltaTime > 0.25)
			{
				deltaTime = 0.25;
			}

			// TODO: Make a scene object instead?
			ImGuiSystem::processInputs(deltaTime, textInputChar);

			// Step begin
			{
				StepEvent e(StepEventType::BEGIN);
				onEvent(&e);
			}

			// Apply time delta to accumulator
			accumulator += deltaTime;
			while(accumulator >= dt)
			{
				// Update the game
				{
					TickEvent e(dt);
					onEvent(&e);
				}
				accumulator -= dt;
			}

			// New ImGui frame
			ImGuiSystem::newFrame();

			// Draw the game
			const double alpha = accumulator / dt;
			{
				DrawEvent e(alpha, dt, graphicsContext);
				onEvent(&e);
			}

			// Draw ImGui last
			ImGuiSystem::render();

			SDL_GL_SwapWindow(mainWindow->getSDLHandle());
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Add fps sample
			if(deltaTime != 0.0f)
			{
				fpsSamples[currFpsSample++] = 1.0 / deltaTime;
				if(currFpsSample >= numFpsSamples)
				{
					currFpsSample = 0;
				}
			}

			// Get average fps
			double fps = 0.0;
			for(uint i = 0; i < numFpsSamples; i++) fps += fpsSamples[i];
			m_framesPerSecond = fps / numFpsSamples;

			// Step end
			{
				StepEvent e(StepEventType::END);
				onEvent(&e);
			}
		}
gameloopend:

		SDL_StopTextInput();
		
		LOG("** Game Ending **");

		// Call onEnd event
		{
			GameEvent e(GameEventType::END);
			onEvent(&e);
		}
	}
	catch(Exception &e)
	{
		stringstream ss;
		ss << e.message() << endl << "------------------------------------------------------------------------------------------------" << endl;
		ss << "Callstack: " << endl << e.callstack();
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "An error occured", ss.str().c_str(), m_windows.front()->getSDLHandle());
		LOG("An exception occured: %s", ss.str().c_str());
		return (uint32)e.errorCode();
	}

	// Free font rendering system
	FontRenderingSystem::free();

	return (uint32)RetCode::SAUCE_OK;
}

void Game::end()
{
	m_running = false;
}

void Game::setPaused(const bool paused)
{
	m_paused = paused;
}

uint Game::getFlags() const
{
	return m_desc.flags;
}

bool Game::isEnabled(const EngineFlag flag)
{
	return (m_desc.flags & (uint32)flag) != 0;
}

Window *Game::getWindow(const Sint32 id) const
{
	if(id < 0)
	{
		return m_windows.front();
	}
	for(Window *window : m_windows)
	{
		if(window->getID() == id)
		{
			return window;
		}
	}
	return 0;
}

END_SAUCE_NAMESPACE
