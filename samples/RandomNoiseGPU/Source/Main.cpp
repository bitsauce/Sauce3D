#include <Sauce/Sauce.h>

using namespace sauce;

class RandomNoiseGPU : public Game
{
	ShaderRef m_noiseShader;
	Texture2DRef m_gradientTexture;
	RenderTarget2DRef m_renderTarget;
	float m_time;

public:
	void onStart(GameEvent *e)
	{
		ShaderDesc fractalShaderDesc;
		fractalShaderDesc.shaderFileVS = "Shaders/Fractal2D.vert";
		fractalShaderDesc.shaderFilePS = "Shaders/Fractal2D.frag";
		m_noiseShader = CreateNew<Shader>(fractalShaderDesc);

		//ShaderDesc fractalShaderDesc;
		//fractalShaderDesc.shaderFileVS = "Shaders/Fractal2D_Gradient.vert";
		//fractalShaderDesc.shaderFilePS = "Shaders/Fractal2D_Gradient.frag";
		//m_noiseShader = CreateNew<Shader>(fractalShaderDesc);

		//ShaderDesc voronoiseShaderDesc;
		//voronoiseShaderDesc.shaderFileVS = "Shaders/Voronoise.vert";
		//voronoiseShaderDesc.shaderFilePS = "Shaders/Voronoise.frag";
		//m_noiseShader = CreateNew<Shader>(voronoiseShaderDesc);

		Texture2DDesc gradientTextureDesc;
		gradientTextureDesc.filePath = "Images/Gradient.png";
		m_gradientTexture = CreateNew<Texture2D>(gradientTextureDesc);
		
		RenderTarget2DDesc renderTargetDesc;
		renderTargetDesc.width = getWindow()->getWidth();
		renderTargetDesc.height = getWindow()->getHeight();
		m_renderTarget = CreateNew<RenderTarget2D>(renderTargetDesc);

		m_noiseShader->setUniform1f("u_Frequency", 0.5f);
		m_noiseShader->setUniform1f("u_Gain", 0.5f);
		m_noiseShader->setUniform1f("u_Lacunarity", 2.0f);
		m_noiseShader->setUniform1i("u_Octaves", 8);
		m_noiseShader->setSampler2D("u_Gradient", m_gradientTexture);

		Game::onStart(e);
	}

	void onEnd(GameEvent *e)
	{
		Game::onEnd(e);
	}

	void onTick(TickEvent *e)
	{
		m_time += e->getDelta();
		Game::onTick(e);
	}
	
	void onDraw(DrawEvent *e)
	{
		GraphicsContext *context = e->getGraphicsContext();

		m_noiseShader->setUniform1f("u_Time", m_time + e->getAlpha() * 1.0f / 30.0f);

		context->pushRenderTarget(m_renderTarget);
		context->setShader(m_noiseShader);
		context->drawRectangle(Vector2F(0, 0), getWindow()->getSize());
		context->setShader(0);
		context->popRenderTarget();

		context->setTexture(m_renderTarget->getTargetTexture());
		context->drawRectangle(Vector2F(0, 0), getWindow()->getSize());
		context->setTexture(0);

		if(getInputManager()->getKeyState(SAUCE_KEY_SPACE))
		{
			Pixmap pixmap = m_renderTarget->getTargetTexture()->getPixmap();
			uchar data[4];
			uint histogram[256] = { 0 };
			for(int y = 0; y < pixmap.getHeight(); y++)
			{
				for(int x = 0; x < pixmap.getWidth(); x++)
				{
					pixmap.getPixel(x, y, data);
					histogram[data[0]]++;
				}
			}
			context->setLineWidth(2);

			uint maxValue = 0;
			for(int i = 0; i < 256; i++)
			{
				maxValue = max(histogram[i], maxValue);
			}

			VertexArray& vertices = context->getTempVertexArray(256);
			for(int i = 0; i < 256; i++)
			{
				vertices[i].set2f(VertexAttribute::Position, i, 256 - (histogram[i] / float(maxValue)) * 256);
				vertices[i].set4ub(VertexAttribute::Color, 255, 0, 0, 255);
			}
			context->drawRectangle(0, 0, 256, 256, Color(0, 0, 0, 200));
			context->drawPrimitives(PrimitiveType::LineStrip, vertices, 256);
		}

		Game::onDraw(e);
	}
};

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
{
	GameDesc desc;
	desc.name = "RandomNoiseGPU Sample";
	desc.workingDirectory = "../Assets";
	desc.flags = (uint32)EngineFlag::ResizableWindow;
	desc.graphicsBackend = GraphicsBackend::OpenGL4;

	RandomNoiseGPU game;
	return game.run(desc);
}