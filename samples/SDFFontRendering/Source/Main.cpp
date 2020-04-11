/* Include the SauceEngine framework */
#include <Sauce/Sauce.h>
using namespace sauce;

class SDFFontRenderingGame : public Game
{
	Resource<Shader> m_fontShader;
	FontRenderer* m_fontRenderer;

public:
	void onStart(GameEvent* e)
	{
		m_fontShader = Resource<Shader>("Shader");
		m_fontRenderer = FontRenderingSystem::createFontRenderer("C:/Windows/Fonts/Arial.ttf");
		Game::onStart(e);
	}

	void onEnd(GameEvent* e)
	{
		Game::onEnd(e);
	}

	void onTick(TickEvent* e)
	{
		Game::onTick(e);
	}
	void onDraw(DrawEvent* e)
	{
		e->getGraphicsContext()->setShader(m_fontShader);
		m_fontRenderer->drawString(e->getGraphicsContext(), "");
		e->getGraphicsContext()->setShader(nullptr);
		Game::onDraw(e);
	}
};

#ifdef __WINDOWS__
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
{
	GameDesc desc;
	desc.name = "SDFFontRendering Sample";
	desc.workingDirectory = "../Assets";
	desc.flags = SAUCE_WINDOW_RESIZABLE;
	desc.graphicsBackend = SAUCE_OPENGL_3;

	SDFFontRenderingGame game;
	return game.run(desc);
}
#else
int main()
{
	SDFFontRenderingGame game;
	return game.run();
}
#endif