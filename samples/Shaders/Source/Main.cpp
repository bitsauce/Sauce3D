#include <Sauce/Sauce.h>

using namespace sauce;

class ShadersGame : public Game
{
	Resource<Shader> shader;
	float time = 0.0f;

public:
	void onStart(GameEvent *e)
	{
		shader = Resource<Shader>("Shader");
		Game::onStart(e);
	}

	void onEnd(GameEvent *e)
	{
		Game::onEnd(e);
	}

	void onTick(TickEvent *e)
	{
		time += e->getDelta();
		Game::onTick(e);
	}

	void onDraw(DrawEvent *e)
	{
		GraphicsContext *context = getWindow()->getGraphicsContext();
		shader->setUniform1f("u_Time", time);
		context->setShader(shader);
		context->drawRectangle(0, 0, getWindow()->getWidth(), getWindow()->getHeight());
		context->setShader(0);
		Game::onDraw(e);
	}
};

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
{
	GameDesc desc;
	desc.name = "Shader Sample";
	desc.workingDirectory = "../Data";
	desc.graphicsBackend = SAUCE_OPENGL_4;

	ShadersGame game;
	return game.run(desc);
}