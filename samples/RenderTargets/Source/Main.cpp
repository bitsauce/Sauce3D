#include <Sauce/Sauce.h>

using namespace sauce;

class RenderTargetsGame : public Game
{
	RenderTarget2D *m_renderTarget;
	RenderTarget2D *m_renderTarget2;
	shared_ptr<Texture2D> m_texture;

public:
	void onStart(GameEvent *e)
	{
		GraphicsContext *graphicsContext = getWindow()->getGraphicsContext();
		m_renderTarget = graphicsContext->createRenderTarget(128, 128);
		m_renderTarget2 = graphicsContext->createRenderTarget(128, 128);
		m_texture = shared_ptr<Texture2D>(getWindow()->getGraphicsContext()->createTexture(Pixmap("Image.png")));
		Game::onStart(e);
	}

	void onDraw(DrawEvent *e)
	{
		GraphicsContext *graphicsContext = e->getGraphicsContext();

		graphicsContext->setTexture(0);
		graphicsContext->pushRenderTarget(m_renderTarget2);
			graphicsContext->clear(GraphicsContext::COLOR_BUFFER);
			graphicsContext->pushRenderTarget(m_renderTarget);
				graphicsContext->clear(GraphicsContext::COLOR_BUFFER);
				graphicsContext->setTexture(m_texture);
				graphicsContext->drawRectangle(0, 0, 64, 64, Color::White, TextureRegion(0.5,0.5,0.9,0.9));
			graphicsContext->popRenderTarget();
			graphicsContext->drawRectangle(64, 0, 64, 64, Color(0, 255, 0, 255));
		graphicsContext->popRenderTarget();

		m_renderTarget->getTexture()->exportToFile("../RenderTarget.png");
		
		graphicsContext->drawRectangle(0, 0, graphicsContext->getWidth(), graphicsContext->getHeight(), Color(127, 127, 127, 255));
		
		graphicsContext->setTexture(m_renderTarget->getTexture());
		graphicsContext->drawRectangle(0, 0, 128, 128);
		graphicsContext->setTexture(m_renderTarget2->getTexture());
		graphicsContext->drawRectangle(0, 0, 128, 128);

		graphicsContext->setTexture(m_renderTarget->getTexture());
		graphicsContext->drawRectangle(300, 300, 64, 64);

		Game::onDraw(e);
	}
};

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
{
	GameDesc desc;
	desc.name = "RenderTargets Sample";
	desc.workingDirectory = "../Data";
	desc.flags = SAUCE_WINDOW_RESIZABLE;
	desc.graphicsBackend = SAUCE_OPENGL_3;

	RenderTargetsGame game;
	return game.run(desc);
}