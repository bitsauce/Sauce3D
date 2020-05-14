/* Include the SauceEngine framework */
#include <Sauce/Sauce.h>

using namespace sauce;

class LandscapeGame : public Game
{
public:
	/** 
	 * onStart():
	 * This event is called after the framework has initialized and before the first tick.
	 * Load resources and set up the game window here.
	 */
	void onStart(GameEvent *e)
	{
		Game::onStart(e);
	}

	/**
	 * onEnd():
	 * This event is called before the framework has fully closed.
	 * Free all resources and save the game state here.
	 */
	void onEnd(GameEvent *e)
	{
		Game::onEnd(e);
	}

	/**
	 * onTick():
	 * This event is called when the framework should update the game state.
	 * Things like physics and gameplay code should go here.
	 */
	void onTick(TickEvent *e)
	{
		Game::onTick(e);
	}

	/**
	 * onDraw():
	 * This event is called when the framework is drawing the current state to the screen.
	 * Use the graphics context provided by e->getGraphicsContext() to draw to the screen. 
	 */
	void onDraw(DrawEvent *e)
	{
		Game::onDraw(e);
	}
};

#ifdef __WINDOWS__
/** Main entry point. This is where our program first starts executing. */
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
{
	/** 
	 * To start the game we simply create a game object and call run().
	 * This function should typically not require modification.
	 */
	GameDesc desc;
	desc.name = "Landscape Sample";
	desc.workingDirectory = "../Assets";
	desc.flags = (uint32)EngineFlag::ResizableWindow;
	desc.graphicsBackend = GraphicsBackend::OpenGL4;

	LandscapeGame game;
	return game.run(desc);
}
#else
int main()
{
	LandscapeGame game;
	return game.run();
}
#endif