/* Include the SauceEngine framework */
#include <Sauce/Sauce.h>

using namespace sauce;

class GraphicsCoreTest : public Game
{
public:
	enum class TestStep
	{
		// Basics
		SingleTriangle,
		Triangles,
		TriangleStrip,
		
		// Shaders
		TriangleWithShader,

		// Texturing
		TexturedTriangle,
		CustomTextureFormat,

		// Custom vertex formats
		CustomVertexFormat,

		// Vertex buffer rendering
		DrawVertexBuffer,

		// Render targets
		DrawRenderTarget
	};

	TestStep m_currentTestStep;

	void onStart(GameEvent* e)
	{
		m_currentTestStep = (TestStep)0;
	}

	void onDraw(DrawEvent* e)
	{
		GraphicsContextRef context = e->getGraphicsContext();
		const Vector2F center = context->getSize() * 0.5f;

		//context->setShader(m_testShader);

		{
			// Get 3 temp vertices
			VertexArray& vertexArray = context->getTempVertexArray(3);

			// Bottom-left - Red vertex
			vertexArray[0].set3f(VertexAttribute::Position, 100.0f, 100.0f, 0.0f);
			vertexArray[0].set4f(VertexAttribute::Color, 1.f, 0.f, 0.f, 1.f);

			// Top-center - Green vertex
			vertexArray[1].set3f(VertexAttribute::Position, 200.0f, 100.0f, 0.0f);
			vertexArray[1].set4f(VertexAttribute::Color, 0.f, 1.f, 0.f, 1.f);

			// Bottom-right - Blue vertex
			vertexArray[2].set3f(VertexAttribute::Position, 100.0f, 200.0f, 0.0f);
			vertexArray[2].set4f(VertexAttribute::Color, 0.f, 0.f, 1.f, 1.f);

			// Draw triangle
			context->drawPrimitives(PrimitiveType::Triangles, vertexArray, 3);
		}

		{
			// Get 3 temp vertices
			VertexArray& vertexArray = context->getTempVertexArray(3);

			// Bottom-left - Red vertex
			vertexArray[0].set3f(VertexAttribute::Position, center.x - 200.0f, center.y + 200.0f, 0.0f);
			vertexArray[0].set4f(VertexAttribute::Color, 1.f, 0.f, 0.f, 1.f);

			// Top-center - Green vertex
			vertexArray[1].set3f(VertexAttribute::Position, center.x + 000.0f, center.y - 200.0f, 0.0f);
			vertexArray[1].set4f(VertexAttribute::Color, 0.f, 1.f, 0.f, 1.f);

			// Bottom-right - Blue vertex
			vertexArray[2].set3f(VertexAttribute::Position, center.x + 200.0f, center.y + 200.0f, 0.0f);
			vertexArray[2].set4f(VertexAttribute::Color, 0.f, 0.f, 1.f, 1.f);

			// Draw triangle
			context->drawPrimitives(PrimitiveType::Triangles, vertexArray, 3);
		}

		//context->setShader(nullptr);

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
	desc.name = "GraphicsCoreTest";
	desc.workingDirectory = "../Assets";
	desc.flags = (uint32)EngineFlag::ResizableWindow;
	desc.graphicsBackend = GraphicsBackend::DirectX12;

	GraphicsCoreTest  game;
	return game.run(desc);
}
#else
int main()
{
	GraphicsCoreTestGame game;
	return game.run();
}
#endif