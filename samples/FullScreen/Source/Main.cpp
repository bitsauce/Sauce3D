#include <Sauce/Sauce.h>

using namespace sauce;

class FullScreenGame : public Game
{
public:
	void onStart(GameEvent*)
	{
	}

	void onEnd(GameEvent*)
	{
	}

	void onTick(TickEvent*)
	{
	}

	void onDraw(DrawEvent*)
	{
	}
};

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
{
	GameDesc desc;
	desc.name = "FullScreen Sample";
	desc.workingDirectory = "../Data";
	desc.flags = SAUCE_WINDOW_RESIZABLE;
	desc.graphicsBackend = SAUCE_OPENGL_3;

	FullScreenGame game;
	return game.run(desc);
}