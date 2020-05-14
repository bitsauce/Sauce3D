/** Include the SauceEngine framework */
#include <Sauce/Sauce.h>
#include <Sauce/ImGui.h>

using namespace sauce;

class SDFFontRenderingGame : public Game
{
	FontRendererRef m_fontRenderer;
	FontRendererDrawTextArgs m_drawArgs;

public:
	void onStart(GameEvent* e)
	{
		FontRendererDesc fontDesc;
		m_fontRenderer = CreateNew<FontRenderer>(fontDesc);

		m_drawArgs.text = "Hello World";
		m_drawArgs.position = Game::getWindow()->getGraphicsContext()->getSize() * 0.5f;
		m_drawArgs.scale = 0.5f;
		m_drawArgs.alignment = TextAlignment::Centered;

		Game::onStart(e);
	}

	void onDraw(DrawEvent* e)
	{
		static const int32 MAX_TEXT_LENGTH = 256;

		GraphicsContext* context = e->getGraphicsContext();
		{
			ImGui::Begin("SDF Font Rendering");

			if (ImGui::TreeNode("Text"))
			{
				static char drawText[MAX_TEXT_LENGTH] = "Hello World";
				ImGui::InputText("Text", drawText, MAX_TEXT_LENGTH);
				m_drawArgs.text = drawText;
				ImGui::SliderFloat("PosX", &m_drawArgs.position.x, 0.f, context->getSize().x);
				ImGui::SliderFloat("PosY", &m_drawArgs.position.y, 0.f, context->getSize().y);
				ImGui::SliderFloat("Scale", &m_drawArgs.scale, 0.0f, 10.0f);
				ImGui::SliderFloat("Rotation", &m_drawArgs.rotation, 0.0f, 360.0f);
				const char* textAlignments[] = { "Left", "Center", "Right" };
				ImGui::Combo("Alignment", (int*)&m_drawArgs.alignment, textAlignments, IM_ARRAYSIZE(textAlignments));
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("SDF"))
			{
				ImGui::SliderFloat("edge0", &m_drawArgs.edge0, 0.0f, 1.0f);
				ImGui::SliderFloat("edge1", &m_drawArgs.edge1, 0.0f, 1.0f);
				ImGui::TreePop();
			}

			ImGui::End();
		}
		
		m_fontRenderer->drawText(context, m_drawArgs);

		Game::onDraw(e);
	}
};

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
{
	GameDesc desc;
	desc.name = "SDFFontRendering Sample";
	desc.workingDirectory = "../Assets";
	desc.flags = (uint32)EngineFlag::ResizableWindow;
	desc.graphicsBackend = GraphicsBackend::OpenGL4;

	SDFFontRenderingGame game;
	return game.run(desc);
}