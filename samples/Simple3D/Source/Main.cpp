#include <Sauce/Sauce.h>
#include "Camera.h"
#include "Mesh.h"
#include "Lights.h"

using namespace sauce;

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
	format.set(VERTEX_POSITION, 3, SAUCE_FLOAT);
	format.set(VERTEX_COLOR, 4, SAUCE_UBYTE);
	format.set(VERTEX_TEX_COORD, 2, SAUCE_FLOAT);
	
	Vertex *vertices = format.createVertices(36);

	Matrix4 mat;
	mat.translate(x, y, z);
	mat.scale(w / 2, h / 2, d / 2);
	for(int i = 0; i < 36; i++) {
		Vector4F pos = mat * CUBE_VERTICES[i];
		Vector2F tex = CUBE_TEX_COORDS[i];
		vertices[i].set3f(VERTEX_POSITION, pos.x, pos.y, pos.z);
		vertices[i].set2f(VERTEX_TEX_COORD, tex.x, tex.y);
		vertices[i].set4ub(VERTEX_COLOR, 255, 255, 255, 255);
	}

	// Draw triangles
	graphicsContext->drawPrimitives(GraphicsContext::PRIMITIVE_TRIANGLES, vertices, 36);

	delete[] vertices;
}

void drawMesh(GraphicsContext* graphicsContext, const float x, const float y, const float z, const float w, const float h, const float d, Mesh *mesh)
{
	//m_defaultShader->setUniformMatrix4f();
	graphicsContext->drawPrimitives(GraphicsContext::PRIMITIVE_TRIANGLES, mesh->getVertices(), mesh->getVertexCount());
}

class Simple3DGame : public Game
{
	Camera camera;
	SpriteBatch *m_spriteBatch;
	Resource<Font> m_font;
	Resource<Shader> m_defaultShader;
	Resource<Texture2D> m_texture;
	
	Mesh *m_mesh;
	DirectionalLight m_directionalLight;
	vector<PointLight> m_pointLights;

	float m_time;

public:
	void onStart(GameEvent *e)
	{
		m_spriteBatch = new SpriteBatch;
		m_defaultShader = Resource<Shader>("Shader/Default");
		m_texture = Resource<Texture2D>("Texture/Sample");
		m_texture->setWrapping(Texture2D::REPEAT);
		m_font = Resource<Font>("Font/Arial");

		addChildLast(&camera);
		camera.setPosition(Vector3F(0.0f, 0.0f, 2.0f));
		camera.setYaw(-math::degToRad(90));

		m_mesh = loadMesh("bunny.obj");
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
		GraphicsContext *graphicsContext = e->getGraphicsContext();
		
		// Push 3D rendering state
		graphicsContext->pushState();
		{
			graphicsContext->enable(GraphicsContext::DEPTH_TEST);
			graphicsContext->enable(GraphicsContext::FACE_CULLING);

			// Set shader
			m_defaultShader->setSampler2D("u_Texture", m_texture);
			//m_defaultShader->setUniform3f("u_DirLight.direction", 0,0,0);
			//m_defaultShader->setUniform3f("u_DirLight.color", 1.0, 0.5, 0.5);
			for(int i = 0; i < m_pointLights.size(); i++)
			{
				const PointLight &light = m_pointLights[i];
				const string uniformPrefix = "u_PointLight[" + std::to_string(i) + "]";
				m_defaultShader->setUniform3f(uniformPrefix + ".position", light.position.x, light.position.y, light.position.z);
				m_defaultShader->setUniform3f(uniformPrefix + ".color", light.color.x, light.color.y, light.color.z);
				m_defaultShader->setUniform1f(uniformPrefix + ".radius", light.radius);
			}
			m_defaultShader->setUniform1i("u_NumPointLights", m_pointLights.size());
			m_defaultShader->setUniformMatrix4f("u_ModelMatrix", Matrix4().get());
			graphicsContext->setShader(m_defaultShader);

			// Set camera and perspective matricies
			const float aspectRatio = float(getWindow()->getWidth()) / getWindow()->getHeight();
			graphicsContext->pushMatrix(graphicsContext->createLookAtMatrix(camera.getDrawPosition(e->getAlpha()), camera.getForwardVector() * -1));
			graphicsContext->setProjectionMatrix(graphicsContext->createPerspectiveMatrix(45.0f, aspectRatio, 0.1f, 100.0f));

			// Draw cube at origo
			//drawCube(graphicsContext, 0, 0, 0, 1, 1, 1);
			drawMesh(graphicsContext, 0, 0, 0, 1, 1, 1, m_mesh);
		}
		graphicsContext->popState();

		// Draw 2D elements here
		stringstream ss;
		ss << "Position: " << camera.getPosition() << endl;
		ss << "Yaw: " << camera.getYaw() << endl;
		ss << "Pitch: " << camera.getPitch() << endl;

		graphicsContext->disable(GraphicsContext::DEPTH_TEST);
		m_spriteBatch->begin(e->getGraphicsContext());
		//m_font->draw(m_spriteBatch, 10, 10, ss.str().c_str(), FONT_ALIGN_LEFT);
		m_spriteBatch->end();

		Game::onDraw(e);
	}
};

/* Main entry point. This is where our program first starts executing. */
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
{
	GameDesc desc;
	desc.name = "Simple3D Sample";
	desc.workingDirectory = "../Data";
	desc.graphicsBackend = SAUCE_OPENGL_4;

	Simple3DGame game;
	return game.run(desc);
}