/* Include the SauceEngine framework */
#include <Sauce/Sauce.h>

using namespace sauce;

class HelloTriangleGame : public Game
{
public:
	void onDraw(DrawEvent *e)
	{
		GraphicsContext* context = e->getGraphicsContext();
		const Vector2F center = context->getSize() * 0.5f;

		// Get 3 temp vertices
		VertexArray& vertexArray = context->getTempVertexArray(3);
		
		// Bottom-left - Red vertex
		vertexArray[0].set3f(VertexAttribute::Position, center.x - 200.0f, center.y + 200.0f, 0.0f);
		vertexArray[0].set4ub(VertexAttribute::Color, 255, 0, 0, 255);

		// Top-center - Green vertex
		vertexArray[1].set3f(VertexAttribute::Position, center.x + 000.0f, center.y - 200.0f, 0.0f);
		vertexArray[1].set4ub(VertexAttribute::Color, 0, 255, 0, 255);

		// Bottom-right - Blue vertex
		vertexArray[2].set3f(VertexAttribute::Position, center.x + 200.0f, center.y + 200.0f, 0.0f);
		vertexArray[2].set4ub(VertexAttribute::Color, 0, 0, 255, 255);

		// Draw triangle
		context->drawPrimitives(PrimitiveType::Triangles, vertexArray, 3);

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
	desc.name = "HelloTriangle Sample";
	desc.workingDirectory = "../Assets";
	desc.flags = (uint32)EngineFlag::ResizableWindow;
	desc.graphicsBackend = GraphicsBackend::OpenGL4;

	HelloTriangleGame game;
	return game.run(desc);
}
#else
int main()
{
	HelloTriangleGame game;
	return game.run();
}
#endif