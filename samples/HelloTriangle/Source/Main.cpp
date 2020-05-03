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

		static Vertex vertices[3];
		
		vertices[0].set2f(VertexAttribute::VERTEX_POSITION, center.x - 200.0f, center.y + 200.0f);
		vertices[0].set4ub(VertexAttribute::VERTEX_COLOR, 255, 0, 0, 255);

		vertices[1].set2f(VertexAttribute::VERTEX_POSITION, center.x + 000.0f, center.y - 200.0f);
		vertices[1].set4ub(VertexAttribute::VERTEX_COLOR, 0, 255, 0, 255);

		vertices[2].set2f(VertexAttribute::VERTEX_POSITION, center.x + 200.0f, center.y + 200.0f);
		vertices[2].set4ub(VertexAttribute::VERTEX_COLOR, 0, 0, 255, 255);

		context->drawPrimitives(PrimitiveType::PRIMITIVE_TRIANGLES, vertices, 3);

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
	desc.flags = (uint32)EngineFlag::SAUCE_WINDOW_RESIZABLE;
	desc.graphicsBackend = GraphicsBackend::SAUCE_OPENGL_3;

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