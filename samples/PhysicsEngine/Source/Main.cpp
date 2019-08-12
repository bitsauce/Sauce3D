#include <Sauce/Sauce.h>

#include "Config.h"
#include "Body.h"
#include "Shapes.h"
#include "Manifold.h"
#include "Collision.h"
#include "Scenes.h"

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
// [ ] Add angle to Box class
// [x] Add oriented polygons
// [x] Add oriented circles
// [ ] Implement a real broadphase
// [ ] Make the drag functionality drag from the clicked point (may require joints)
// [ ] Fix sinking bug with polygon <-> circle
// [ ] Implement "mass = volume * density" mass initialization
// [ ] Fix drifting with stacked boxes (maybe caused by low mass?)
// [ ] Do optimization
// [ ] Add grid to grid collision
// [ ] Optimize the rendering
// [ ] Finetune CircleToCircle
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
	SceneManager m_scene;
	Body *m_selectedBody;
	Vector2F m_lastMousePosition;
	function<void(Manifold*)> m_manifoldGenerationFunctionTable[Shape::NUM_SHAPES][Shape::NUM_SHAPES];

	vector<Manifold> m_manifoldPool;
	uint m_numManifolds;

	Vector2F m_gravity = Vector2F(0.0f, 150.0f);
	//Vector2F m_gravity = Vector2F(0.0f, 0.0f);

	SpriteBatch *m_spriteBatch;
	Resource<Font> m_font;

	const int m_manifoldPoolChunkSize = 10000;

public:
	void onStart(GameEvent *e)
	{
		m_spriteBatch = new SpriteBatch;
		m_font = Resource<Font>("Arial");

		m_selectedBody = nullptr;
		
		m_scene.initialize(g_initialScene);

		m_manifoldPool.resize(m_manifoldPoolChunkSize);
		
		getWindow()->getGraphicsContext()->setPointSize(5.0f);

		m_manifoldGenerationFunctionTable[Shape::CIRCLE][Shape::CIRCLE] = collision::CircleToCircle;
		m_manifoldGenerationFunctionTable[Shape::CIRCLE][Shape::POLYGON] = collision::CircleToPolygon;
		m_manifoldGenerationFunctionTable[Shape::POLYGON][Shape::CIRCLE] = collision::PolygonToCircle;
		m_manifoldGenerationFunctionTable[Shape::POLYGON][Shape::POLYGON] = collision::PolygonToPolygon;

		Game::onStart(e);

#if RUN_AS_BENCHMARK == 1
		new std::thread([]() {
			std::this_thread::sleep_for(10s);
			exit(0);
		});
#endif
	}

	void onEnd(GameEvent *e)
	{
		Game::onEnd(e);
	}

	void onTick(TickEvent *e)
	{
		vector<Body*> &m_bodies = m_scene.getBodies();

		const float timeScale = 1.0f;
		const float dt = e->getDelta() * timeScale;

		// Apply velocity to selected shape
		if(m_selectedBody)
		{
			m_selectedBody->setVelocity((getInputManager()->getPosition() - m_selectedBody->getPosition()) * 0.25f / e->getDelta());
			m_selectedBody->setAngularVelocity(0.0f);
		}

		// Apply gravity to all bodies
		for(Body *body : m_bodies)
		{
			if(!body->isStatic())
			{
				body->setVelocity(body->getVelocity() + m_gravity * dt);
			}

			body->m_isColliding = false;
			for(Shape *shape : body->getShapes())
				shape->debugPoints.clear();
		}

		m_numManifolds = 0;

		// Broadphase - Find and resolve all colliding shapes
		for(int i = 0; i < m_bodies.size(); i++)
		{
			Body *body = m_bodies[i];
			for(int j = i + 1; j < m_bodies.size(); j++)
			{
				Body *otherBody = m_bodies[j];

				// Skip collision between static objects
				if(body->isStatic() && otherBody->isStatic())
					continue;

				// Check collision on every shape of each body
				vector<Shape*> &shapes = body->getShapes();
				vector<Shape*> &otherShapes = otherBody->getShapes();
				for(int k = 0; k < shapes.size(); k++)
				{
					for(int j = 0; j < otherShapes.size(); j++)
					{
						Shape *shape = shapes[k];
						Shape *otherShape = otherShapes[j];
						
						if(m_numManifolds >= m_manifoldPool.size())
						{
							m_manifoldPool.resize(m_manifoldPool.size() + m_manifoldPoolChunkSize);
						}

						Manifold &manifold = m_manifoldPool[m_numManifolds++];
						manifold.initialize(body, otherBody, shape, otherShape);
						m_manifoldGenerationFunctionTable[shape->getType()][otherShape->getType()](&manifold);
						if(manifold.numContacts > 0)
						{
#if DISABLE_COLLISIONS != 1
							ResolveCollision(&manifold);
							CorrectPositions(&manifold);
#endif
							body->m_isColliding = otherBody->m_isColliding = true;
						}
					}
				}
			}
		}

		for(Body *body : m_bodies)
		{
			body->move(body->getVelocity() * dt);
			body->rotate(body->getAngularVelocity() * dt);
		}

		Game::onTick(e);
	}

	void onDraw(DrawEvent *e)
	{
		vector<Body*> &m_bodies = m_scene.getBodies();

		for(Body *body : m_bodies)
		{
			Color color = body->m_isColliding ? Color::Blue : Color::White;
			
			// Render shape
			//Color color = Color::White;
			body->draw(e->getGraphicsContext(), color, e->getAlpha());

			// Draw velocity arrow
			e->getGraphicsContext()->drawArrow(body->getPosition(), body->getPosition() + body->getVelocity() * 0.1f, Color::Red);
		}

		for(int i = 0; i < m_numManifolds; ++i)
		{
			Manifold &m = m_manifoldPool[i];
			for(int i = 0; i < m.numContacts; ++i)
			{
				const Vector2F contactPoint = m.getContactPoint(i);

				Vertex v;
				v.set2f(VERTEX_POSITION, contactPoint.x, contactPoint.y);
				v.set4ub(VERTEX_COLOR, 255, 255, 0, 255);
				e->getGraphicsContext()->drawPrimitives(GraphicsContext::PRIMITIVE_POINTS, &v, 1);
				e->getGraphicsContext()->drawArrow(contactPoint, contactPoint + m.normal * 15.0f, Color::Red);
			}
		}

		stringstream debugStr;
		debugStr << "FPS: " << getFPS() << "\n";
		debugStr << "# Bodies: " << m_bodies.size() << "\n";

		m_spriteBatch->begin(e->getGraphicsContext());
		m_font->draw(m_spriteBatch, Vector2F(10.0f, 10.0f), debugStr.str());
		m_spriteBatch->end();

		Game::onDraw(e);
	}

	void ResolveCollision(Manifold *m)
	{
		for(int i = 0; i < m->numContacts; ++i)
		{
			const Vector2F contactPoint = m->getContactPoint(i);

			// Get manifold shapes
			Body *a = m->bodyA;
			Body *b = m->bodyB;

			Vector2F rAP = contactPoint - a->getPosition();
			Vector2F rBP = contactPoint - b->getPosition();

			// Calculate relative velocity
			Vector2F relativeVelocity =
				b->getVelocity() + perp(rBP) * b->getAngularVelocity() -
				a->getVelocity() - perp(rAP) * a->getAngularVelocity();

			// Calculate relative velocity in terms of the normal direction
			float velocityAlongNormal = relativeVelocity.dot(m->normal);

			// Do not resolve if velocities are separating
			if(velocityAlongNormal >= 0.0f)
				return;

			// Calculate restitution
			float e = min(a->getRestitution(), b->getRestitution());

			// Calculate impulse denominator
			float rAPcrossN = rAP.cross(m->normal);
			float rBPcrossN = rBP.cross(m->normal);
			float sumInvMass = a->getMassInv() + b->getMassInv() + (rAPcrossN * rAPcrossN * a->getInertiaInv()) + (rBPcrossN * rBPcrossN * b->getInertiaInv());

			// Calculate impulse scalar
			float j = -(1.0f + e) * velocityAlongNormal;
			j /= sumInvMass;
			j /= m->numContacts;

			// Apply normal impulse
			Vector2F impulse = m->normal * j;
			a->applyImpulse(-impulse, rAP);
			b->applyImpulse(impulse, rBP);
			
			// Calculate relative velocity
			relativeVelocity =
				b->getVelocity() + perp(rBP) * b->getAngularVelocity() -
				a->getVelocity() - perp(rAP) * a->getAngularVelocity();

			// Find a tangent in the direction of the relative velocity
			Vector2F tangent = relativeVelocity - m->normal * relativeVelocity.dot(m->normal);
			tangent.normalize();

			// Solve for magnitude to apply along the friction vector
			float jt = -relativeVelocity.dot(tangent);
			jt /= sumInvMass;
			jt /= m->numContacts;

			// Approximate mu given friction coefficients of each body
			float mu = std::sqrt(a->getStaticFriction() * a->getStaticFriction() +
								 b->getStaticFriction() * b->getStaticFriction());

			// Clamp magnitude of friction and calculate impulse vector
			Vector2F frictionImpulse;
			if(abs(jt) < j * mu)
			{
				frictionImpulse = tangent * jt;
			}
			else
			{
				mu = std::sqrt(a->getDynamicFriction() * a->getDynamicFriction() +
							   b->getDynamicFriction() * b->getDynamicFriction());
				frictionImpulse = tangent * -j * mu;
			}

			// Apply friction
			a->applyImpulse(-frictionImpulse, rAP);
			b->applyImpulse(frictionImpulse, rBP);
		}
	}

	void CorrectPositions(Manifold *m)
	{
		// Get manifold shapes
		Body *a = m->bodyA;
		Body *b = m->bodyB;

		// Correct the positions
		const float slop = 0.01;    // Usually 0.01 to 0.1
		const float percent = 0.5f; // Usually 20% to 80%
		Vector2F correction = m->normal * percent * max(m->penetration - slop, 0.0f) / (a->getMassInv() + b->getMassInv());
		a->setPosition(a->getPosition() - correction * a->getMassInv());
		b->setPosition(b->getPosition() + correction * b->getMassInv());
	}

	void onMouseDown(MouseEvent *e)
	{
		vector<Body*> &m_bodies = m_scene.getBodies();
		Vector2F inputPos = e->getPosition();
		for(Body *body : m_bodies)
		{
			if(body->contains(inputPos))
			{
				m_selectedBody = body;
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
		m_lastMousePosition = e->getPosition();
	}

	void onMouseWheel(MouseEvent *e)
	{
		if(m_selectedBody)
		{
			const float rotationalSpeed = math::degToRad(5.0f);
			m_selectedBody->rotate(e->getWheelY() * rotationalSpeed);
		}
	}

	void onKeyDown(KeyEvent *e)
	{
		vector<Body*> &m_bodies = m_scene.getBodies();

		switch(e->getKeycode())
		{
			case Keycode::SAUCE_KEY_C:
			{
				Body *body = new Body();
				body->setPosition(e->getInputManager()->getPosition());
				{
					Circle *circle = new Circle;
					circle->setRadius(25.0f);
					body->addShape(circle);
				}

				if(e->getModifiers() & KeyEvent::SHIFT)
				{
					body->setMass(0.0f);
				}

				m_bodies.push_back(body);
			}
			break;

			case Keycode::SAUCE_KEY_B:
			{
				Body *body = new Body();
				body->setPosition(e->getInputManager()->getPosition());
				{
					Box *box = new Box;
					box->setLocalPosition(Vector2F(0.0f, 0.0f));
					box->setSize(Vector2F(50.0f, 50.0f));
					body->addShape(box);
				}

				if(e->getModifiers() & KeyEvent::SHIFT)
				{
					body->setMass(0.0f);
				}

				m_bodies.push_back(body);
			}
			break;

			case Keycode::SAUCE_KEY_N:
			{
				Body *body = new Body();
				body->setPosition(e->getInputManager()->getPosition());
				{
					Box *box = new Box;
					box->setLocalPosition(Vector2F(0.0f, 0.0f));
					box->setSize(Vector2F(50.0f, 50.0f));
					body->addShape(box);

					Box *box2 = new Box;
					box2->setLocalPosition(Vector2F(200.0f, 0.0f));
					box2->setSize(Vector2F(50.0f, 50.0f));
					body->addShape(box2);
				}

				if(e->getModifiers() & KeyEvent::SHIFT)
				{
					body->setMass(0.0f);
				}

				m_bodies.push_back(body);
			}
			break;

			case Keycode::SAUCE_KEY_P:
			{
				Body *body = new Body();
				body->setPosition(e->getInputManager()->getPosition());
				{
					PolygonShape *polygon = new PolygonShape;
					const Vector2F points[] = {
						Vector2F(-25.0f, -25.0f),
						Vector2F(25.0f, 25.0f),
						Vector2F(-25.0f, 25.0f),
					};
					polygon->initialize(points, 3);
					body->addShape(polygon);
				}

				if(e->getModifiers() & KeyEvent::SHIFT)
				{
					body->setMass(0.0f);
				}

				m_bodies.push_back(body);
			}
			break;

			case Keycode::SAUCE_KEY_O:
			{
				Body *body = new Body();
				body->setPosition(e->getInputManager()->getPosition());
				{
					PolygonShape *polygon = new PolygonShape;
					const Vector2F points[] = {
						Vector2F(-25.0f, -25.0f),
						Vector2F(35.0f, -30.0f),
						Vector2F(25.0f, 25.0f),
						Vector2F(-30.0f, -15.0f)
					};
					polygon->initialize(points, 4);
					body->addShape(polygon);
				}

				if(e->getModifiers() & KeyEvent::SHIFT)
				{
					body->setMass(0.0f);
				}

				m_bodies.push_back(body);
			}
			break;
			
			case Keycode::SAUCE_KEY_1:
			{
				m_scene.initialize(SceneManager::SCENE_ENCLOSURE);
			}
			break;

			case Keycode::SAUCE_KEY_2:
			{
				m_scene.initialize(SceneManager::SCENE_BENCHMARK_CIRCLES);
				new std::thread([]() {
					std::this_thread::sleep_for(5s);
					exit(0);
				});
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

	PhysicsEngineGame game;
	return game.run(desc);
}