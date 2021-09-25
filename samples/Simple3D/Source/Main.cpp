/** Include the SauceEngine framework */
#include <Sauce/Sauce.h>
#include <Sauce/ImGui.h>

#include "Camera.h"
#include "Mesh.h"
#include "Lights.h"

using namespace sauce;

// TODO:
// [ ] Implement a way to pass struct uniforms to shader more easily
// [ ] Move classes that are general purpose (Mesh, maybe Lights, Camera, and drawCube) to Sauce3D
// [ ] Fix ImGui backface culling
// [ ] Make GraphicsContext's "enable/disable" states push/pop-able

Vector4F CUBE_VERTICES[36] = {
	// Back
	Vector4F(-1.0f, -1.0f, -1.0f, 1.0f),
	Vector4F(-1.0f,  1.0f, -1.0f, 1.0f),
	Vector4F( 1.0f, -1.0f, -1.0f, 1.0f),
	Vector4F( 1.0f,  1.0f, -1.0f, 1.0f),
	Vector4F( 1.0f, -1.0f, -1.0f, 1.0f),
	Vector4F(-1.0f,  1.0f, -1.0f, 1.0f),
	
	// Front
	Vector4F(-1.0f, -1.0f, 1.0f, 1.0f),
	Vector4F( 1.0f, -1.0f, 1.0f, 1.0f),
	Vector4F(-1.0f,  1.0f, 1.0f, 1.0f),
	Vector4F( 1.0f,  1.0f, 1.0f, 1.0f),
	Vector4F(-1.0f,  1.0f, 1.0f, 1.0f),
	Vector4F( 1.0f, -1.0f, 1.0f, 1.0f),

	// Right
	Vector4F(-1.0f, -1.0f, -1.0f, 1.0f),
	Vector4F(-1.0f, -1.0f,  1.0f, 1.0f),
	Vector4F(-1.0f,  1.0f, -1.0f, 1.0f),
	Vector4F(-1.0f,  1.0f,  1.0f, 1.0f),
	Vector4F(-1.0f,  1.0f, -1.0f, 1.0f),
	Vector4F(-1.0f, -1.0f,  1.0f, 1.0f),
	
	// Left
	Vector4F(1.0f, -1.0f, -1.0f, 1.0f),
	Vector4F(1.0f,  1.0f, -1.0f, 1.0f),
	Vector4F(1.0f, -1.0f,  1.0f, 1.0f),
	Vector4F(1.0f,  1.0f,  1.0f, 1.0f),
	Vector4F(1.0f, -1.0f,  1.0f, 1.0f),
	Vector4F(1.0f,  1.0f, -1.0f, 1.0f),
	
	// Top
	Vector4F(-1.0f, 1.0f, -1.0f, 1.0f),
	Vector4F(-1.0f, 1.0f,  1.0f, 1.0f),
	Vector4F( 1.0f, 1.0f, -1.0f, 1.0f),
	Vector4F( 1.0f, 1.0f,  1.0f, 1.0f),
	Vector4F( 1.0f, 1.0f, -1.0f, 1.0f),
	Vector4F(-1.0f, 1.0f,  1.0f, 1.0f),

	// Bottom
	Vector4F(-1.0f, -1.0f, -1.0f, 1.0f),
	Vector4F( 1.0f, -1.0f, -1.0f, 1.0f),
	Vector4F(-1.0f, -1.0f,  1.0f, 1.0f),
	Vector4F( 1.0f, -1.0f,  1.0f, 1.0f),
	Vector4F(-1.0f, -1.0f,  1.0f, 1.0f),
	Vector4F( 1.0f, -1.0f, -1.0f, 1.0f)
};

Vector2F CUBE_TEX_COORDS[36] = {
	// Back
	Vector2F(1.0f, 0.0f),
	Vector2F(1.0f, 1.0f),
	Vector2F(0.0f, 0.0f),
	Vector2F(0.0f, 1.0f),
	Vector2F(0.0f, 0.0f),
	Vector2F(1.0f, 1.0f),

	// Front
	Vector2F(0.0f, 0.0f),
	Vector2F(1.0f, 0.0f),
	Vector2F(0.0f, 1.0f),
	Vector2F(1.0f, 1.0f),
	Vector2F(0.0f, 1.0f),
	Vector2F(1.0f, 0.0f),

	// Right
	Vector2F(0.0f, 0.0f),
	Vector2F(1.0f, 0.0f),
	Vector2F(0.0f, 1.0f),
	Vector2F(1.0f, 1.0f),
	Vector2F(0.0f, 1.0f),
	Vector2F(1.0f, 0.0f),

	// Left
	Vector2F(1.0f, 0.0f),
	Vector2F(1.0f, 1.0f),
	Vector2F(0.0f, 0.0f),
	Vector2F(0.0f, 1.0f),
	Vector2F(0.0f, 0.0f),
	Vector2F(1.0f, 1.0f),

	// Top
	Vector2F(0.0f, 0.0f),
	Vector2F(0.0f, 1.0f),
	Vector2F(1.0f, 0.0f),
	Vector2F(1.0f, 1.0f),
	Vector2F(1.0f, 0.0f),
	Vector2F(0.0f, 1.0f),

	// Bottom
	Vector2F(0.0f, 0.0f),
	Vector2F(1.0f, 0.0f),
	Vector2F(0.0f, 1.0f),
	Vector2F(1.0f, 1.0f),
	Vector2F(0.0f, 1.0f),
	Vector2F(1.0f, 0.0f)
};

void drawCube(GraphicsContext* graphicsContext, const float x, const float y, const float z, const float w, const float h, const float d)
{
	VertexFormat format;
	format.set(VertexAttribute::Position, 3, Datatype::Float);
	format.set(VertexAttribute::Color, 4, Datatype::Uint8);
	format.set(VertexAttribute::TexCoord, 2, Datatype::Float);
	
	VertexArray vertices = format.createVertices(36);

	Matrix4 mat;
	mat.translate(x, y, z);
	mat.scale(w / 2, h / 2, d / 2);
	for(int i = 0; i < 36; i++) {
		Vector4F pos = mat * CUBE_VERTICES[i];
		Vector2F tex = CUBE_TEX_COORDS[i];
		vertices[i].set3f(VertexAttribute::Position, pos.x, pos.y, pos.z);
		vertices[i].set2f(VertexAttribute::TexCoord, tex.x, tex.y);
		vertices[i].set4ub(VertexAttribute::Color, 255, 255, 255, 255);
	}

	// Draw triangles
	graphicsContext->drawPrimitives(PrimitiveType::Triangles, vertices, 36);
}

void drawMesh(GraphicsContextRef graphicsContext, const float x, const float y, const float z, const float w, const float h, const float d, Mesh *mesh)
{
	//m_phongShader->setUniformMatrix4f();
	graphicsContext->drawPrimitives(PrimitiveType::Triangles, mesh->getVertexBuffer());
}

class Simple3DGame : public Game
{
	Camera camera;
	DirectionalLight m_directionalLight;
	vector<PointLight> m_pointLights;

	ShaderRef    m_phongShader = nullptr;
	Texture2DRef m_texture       = nullptr;
	MeshRef      m_mesh          = nullptr;

	float m_time = 0.0f;

public:
	void onStart(GameEvent *e)
	{
		ShaderDesc shaderDesc;
		shaderDesc.shaderFileVS = "Phong.vert";
		shaderDesc.shaderFilePS = "Phong.frag";
		m_phongShader = CreateNew<Shader>(shaderDesc);

		Texture2DDesc textureDesc;
		textureDesc.filePath = "Texture.png";
		textureDesc.wrapping = TextureWrapping::Repeat;
		m_texture = CreateNew<Texture2D>(textureDesc);

		MeshDesc meshDesc;
		meshDesc.meshFilePath = "Bunny.obj";
		m_mesh = CreateNew<Mesh>(meshDesc);

		addChildLast(&camera);
		camera.setPosition(Vector3F(-0.55f, 1.80f, 3.30f));
		camera.setYaw(-math::degToRad(85));
		camera.setPitch(-math::degToRad(20));

		m_time = 0.0f;

		m_pointLights.push_back(PointLight(Vector3F(0.f, 0.f, 2.f), Vector3F(1.f, 1.f, 1.f), 10.f));

		Game::onStart(e);
	}

	void onEnd(GameEvent *e)
	{
		Game::onEnd(e);
	}

	void onTick(TickEvent *e)
	{
		m_time += e->getDelta();
		for(int i = 0; i < m_pointLights.size(); i++)
		{
			PointLight &light = m_pointLights[i];
			light.position = Vector3F(cos(m_time) * 5, 1, sin(m_time) * 5);
		}
		Game::onTick(e);
	}

	void onDraw(DrawEvent *e)
	{
		GraphicsContextRef graphicsContext = e->getGraphicsContext();
		
		// Push 3D rendering state
		graphicsContext->pushState();
		{
			graphicsContext->enable(Capability::DepthTest);
			graphicsContext->enable(Capability::FaceCulling);

			// Set shader
			m_phongShader->setSampler2D("u_Texture", m_texture);
			//m_phongShader->setUniform3f("u_DirLight.direction", 0,0,0);
			//m_phongShader->setUniform3f("u_DirLight.color", 1.0, 0.5, 0.5);
			m_phongShader->setUniformStruct("u_PointLight", (uint8*)m_pointLights.data());
			m_phongShader->setUniform1i("u_NumPointLights", m_pointLights.size());
			m_phongShader->setUniformMatrix4f("u_ModelMatrix", Matrix4().get());
			graphicsContext->setShader(m_phongShader);

			// Set camera and perspective matricies
			const float aspectRatio = float(getWindow()->getWidth()) / getWindow()->getHeight();
			graphicsContext->pushMatrix(graphicsContext->createLookAtMatrix(camera.getDrawPosition(e->getAlpha()), camera.getForwardVector() * -1));
			graphicsContext->setProjectionMatrix(graphicsContext->createPerspectiveMatrix(45.0f, aspectRatio, 0.1f, 100.0f));

			// Draw cube at origo
			//drawCube(graphicsContext, 0, 0, 0, 1, 1, 1);
			drawMesh(graphicsContext, 0, 0, 0, 1, 1, 1, &*m_mesh);
		}
		graphicsContext->popState();

		// Draw UI
		graphicsContext->disable(Capability::DepthTest);
		//graphicsContext->disable(Capability::FaceCulling);
		graphicsContext->disable(Capability::Vsync);
		{
			ImGui::Begin("Simple 3D");
			ImGui::Text("FPS: %.2f", Game::Get()->getFPS());

			if (ImGui::TreeNode("Camera"))
			{
				ImGui::Text("Pos: [%.2f, %.2f, %.2f]", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
				ImGui::Text("Yaw: %.2f", camera.getYaw());
				ImGui::Text("Pitch: %.2f", camera.getPitch());
				ImGui::TreePop();
			}

			ImGui::End();
		}

		Game::onDraw(e);
	}
};

/* Main entry point. This is where our program first starts executing. */
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
{
	GameDesc desc;
	desc.name = "Simple3D Sample";
	desc.workingDirectory = "../Assets";
	//desc.graphicsBackend = GraphicsBackend::OpenGL4;
	desc.graphicsBackend = GraphicsBackend::DirectX12;

	Simple3DGame game;
	return game.run(desc);
}