/* Include the SauceEngine framework */
#include <Sauce/Sauce.h>
#include <Sauce/ImGui.h>
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
		//{
		//	static float f = 0.0f;
		//	static int counter = 0;
		//	static bool show_demo_window = false;
		//	static bool show_another_window = false;
		//	static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		//	ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		//	ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		//	ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		//	ImGui::Checkbox("Another Window", &show_another_window);

		//	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		//	ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		//	if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		//		counter++;
		//	ImGui::SameLine();
		//	ImGui::Text("counter = %d", counter);

		//	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		//	ImGui::End();
		//}

		static float edge0 = 0.213f;
		static float edge1 = 0.504f;
		static float scale = 1.0f;
		{
			static int counter = 0;
			static bool show_demo_window = false;
			static bool show_another_window = false;
			static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

			ImGui::Begin("SDF Font Rendering");
			ImGui::SliderFloat("edge0", &edge0, 0.0f, 1.0f);
			ImGui::SliderFloat("edge1", &edge1, 0.0f, 1.0f);
			ImGui::SliderFloat("scale", &scale, 0.0f, 10.0f);
			ImGui::End();
		}

		e->getGraphicsContext()->setShader(m_fontShader);
		m_fontShader->setUniform1f("u_Edge0", edge0);
		m_fontShader->setUniform1f("u_Edge1", edge1);
		m_fontShader->setUniform1f("u_Scale", scale);
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
	desc.flags = (uint32)EngineFlag::SAUCE_WINDOW_RESIZABLE;
	desc.graphicsBackend = GraphicsBackend::SAUCE_OPENGL_3;

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