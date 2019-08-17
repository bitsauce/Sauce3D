#include <Sauce/Sauce.h>

#include "Config.h"
#include "Constants.h"
#include "Body.h"
#include "Shapes.h"
#include "Manifold.h"
#include "Collision.h"
#include "Scenes.h"
#include "PhysicsWorld.h"

using namespace sauce;

// TODO:
// [x] Add static objects
// [x] Fix sinking
// [x] Verify that integration is implemented correctly (hint: delta time?)
// [x] Should impulses be multiplied with delta (maybe not as an impulse is technically not the same as acceleration?)
// [x] Add gravity
// [x] Add a gravity scale variable
// [x] Add friction
// [x] Add oriented boxes
// [x] Fix the memory leak (generalize boxes as polygons)
// [x] Add angle to Box class (maybe just rotate the box before init?)
// [x] Add oriented polygons
// [x] Add oriented circles
// [x] Add AABB generation for circles
// [x] Implement a real broadphase
// [x] Fix sinking bug with polygon <-> circle
// [x] Finetune CircleToCircle
// [x] Finetune PolygonToPolygon
// [x] Optimize the rendering
// [x] Fix AABB generation
// [x] Add a physics unit variable, to scale world values to a space that is better suited for physics updates
// [ ] Implement "mass = volume * density" mass initialization
// [x] Fix shaking boxes
// [ ] Add c.bias
// [ ] Add iterations
// [ ] Fix drifting with stacked boxes (maybe caused by low mass or floating point errors due to e.g. matrix multiplications in collision code?)
// [ ] Fix objects going through other bodies (use RayCasting)
// [ ] Add joints/contraints
// [ ] Make the drag functionality drag from the clicked point (may require joints)
// [ ] Add sleeping
// [ ] Create test vehicle
// [ ] Add grid to grid collision
// [ ] Consider adding ImGui

/*
	Rock       Density : 0.6  Restitution : 0.1
	Wood       Density : 0.3  Restitution : 0.2
	Metal      Density : 1.2  Restitution : 0.05
	BouncyBall Density : 0.3  Restitution : 0.8
	SuperBall  Density : 0.3  Restitution : 0.95
	Pillow     Density : 0.1  Restitution : 0.2
	Static     Density : 0.0  Restitution : 0.4
*/

class PhysicsEngineGame : public Game
{
	// Physics world
	PhysicsWorld m_world;

	// Handles setup of example scenes
	SceneManager m_scene;

	// Font rendering
	SpriteBatch *m_spriteBatch;
	Resource<Font> m_font;

	// Selected body
	Body *m_selectedBody;
	Vector2F m_lastMousePosition;

public:
	void onStart(GameEvent *e)
	{
		// Load font rendering
		m_spriteBatch = new SpriteBatch;
		m_font = Resource<Font>("DebugFont");
		
		// Initialize physics world
		m_world.initialize();

		// Setup example scene
		m_scene.initialize(g_initialScene, m_world.getBodies(), m_world.getPhysicsGrid());

		m_selectedBody = nullptr;

#if RUN_AS_BENCHMARK == 1
		new std::thread([]() {
			std::this_thread::sleep_for(10s);
			exit(0);
		});
#endif // RUN_AS_BENCHMARK == 1

		Game::onStart(e);
	}

	void onEnd(GameEvent *e)
	{
		Game::onEnd(e);
	}

	void onTick(TickEvent *e)
	{
		// Apply velocity to selected shape
		if(m_selectedBody)
		{
			InputManager *inputManager = Game::Get()->getInputManager();
			m_selectedBody->setVelocity((Vector2FInPhysicsSpace(inputManager->getPosition()) - m_selectedBody->getPosition()) * 0.25f / e->getDelta());
			m_selectedBody->setAngularVelocity(0.0f);
		}

		m_world.update(e->getDelta());

		Game::onTick(e);
	}

	void onDraw(DrawEvent *e)
	{
		// Scale visualization of physics bodies
		Matrix4 physicsUnitsToWorld;
		physicsUnitsToWorld.scale(g_physicsUnit);
		e->getGraphicsContext()->pushMatrix(physicsUnitsToWorld);

		for(Body *body : m_world.getBodies())
		{
			Color color = body->m_isColliding ? COLLIDED_POLYGON_COLOR : Color::White;
			
			// Render shape
			//Color color = Color::White;
			body->draw(e->getGraphicsContext(), color, e->getAlpha());

#if DRAW_VELOCITIES == 1
			// Draw velocity arrow
			e->getGraphicsContext()->drawArrow(body->getPosition(), body->getPosition() + body->getVelocity() * 0.1f, ValueInPhysicsSpace(10.0f), Color::Red);
#endif // DRAW_VELOCITIES
		}

#if DRAW_IMPULSES == 1
		for(ManifoldItr itr = m_manifolds.begin(); itr != m_manifolds.end(); ++itr)
		{
			const Manifold &m = itr->second;
			for(int i = 0; i < m.numContacts; ++i)
			{
				const Contact &c = m.contacts[i];

				Vertex v;
				v.set2f(VERTEX_POSITION, c.position.x, c.position.y);
				v.set4ub(VERTEX_COLOR, 255, 255, 0, 255);

				e->getGraphicsContext()->drawPrimitives(GraphicsContext::PRIMITIVE_POINTS, &v, 1);
				e->getGraphicsContext()->drawArrow(c.position, c.position + m.normal * ValueInPhysicsSpace(15.0f) * m.penetration * 2.0f, ValueInPhysicsSpace(10.0f), Color::Red);
			}
		}
#endif // DRAW_IMPULSES

		e->getGraphicsContext()->popMatrix();

		stringstream debugStr;
		debugStr << "FPS: " << getFPS() << "\n";
		debugStr << "# Bodies: " << m_world.getBodies().size() << "\n";

		m_spriteBatch->begin(e->getGraphicsContext());
		m_font->draw(m_spriteBatch, Vector2F(30.0f, 30.0f), debugStr.str());
		m_spriteBatch->end();

		Game::onDraw(e);
	}

	void onMouseDown(MouseEvent *e)
	{
		const Vector2F inputPos = Vector2FInPhysicsSpace(e->getPosition());
		for(Body *body : m_world.getBodies())
		{
			if(body->contains(inputPos))
			{
				m_selectedBody = body;
				m_selectedBody->setAngularVelocity(0.0f);
				break;
			}
		}
	}

	void onMouseUp(MouseEvent *e)
	{
		m_selectedBody = nullptr;
	}

	void onMouseMove(MouseEvent *e)
	{
		m_lastMousePosition = Vector2FInPhysicsSpace(e->getPosition());
	}

	void onMouseWheel(MouseEvent *e)
	{
		if(m_selectedBody)
		{
			const float rotationalSpeed = math::degToRad(5.0f);
			m_selectedBody->setAngle(m_selectedBody->getAngle() + e->getWheelY() * rotationalSpeed);
		}
	}

	void onKeyDown(KeyEvent *e)
	{
		switch(e->getKeycode())
		{			
			case Keycode::SAUCE_KEY_R:
			{
				m_scene.initialize(g_initialScene, m_world.getBodies(), m_world.getPhysicsGrid());
			}
			break;
		}
	}
};

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
{
	GameDesc desc;
	desc.name = "PhysicsEngine Sample";
	desc.workingDirectory = "../Data";
	desc.flags = SAUCE_WINDOW_RESIZABLE;
	desc.graphicsBackend = SAUCE_OPENGL_3;
	desc.deltaTime = g_deltaTime;

	PhysicsEngineGame game;
	return game.run(desc);
}