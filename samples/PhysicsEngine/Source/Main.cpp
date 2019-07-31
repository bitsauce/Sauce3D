#include <Sauce/Sauce.h>
#include "Shapes.h"
#include "Manifold.h"

using namespace sauce;

// TODO:
// [x] Add static objects
// [ ] Fix sinking
// [ ] Verify that integration is implemented correctly (hint: delta time?)
// [x] Add gravity
// [ ] Add a gravity scale variable
// [ ] Consider adding ImGui
// [ ] Add friction
// [ ] Add rotated boxes
// [ ] Add rotated circles
// [ ] Add rotated polygons

namespace manifolds
{
	void CircleToCircle(Manifold *m)
	{
		Circle *a = dynamic_cast<Circle*>(m->a);
		Circle *b = dynamic_cast<Circle*>(m->b);

		// Calculate vector from a to b
		Vector2F normal = b->getCenter() - a->getCenter();
		float lengthSquared = normal.lengthSquared();
		float totalRadius = a->getRadius() + b->getRadius();

		// If their combined radius is less than the distance between,
		// there is no contact
		if(lengthSquared >= totalRadius * totalRadius)
		{
			return;
		}

		// There is contact, calculate the distance using sqrt
		float distance = std::sqrt(lengthSquared);
		m->contactCount += 1;
		if(distance == 0.0f)
		{
			m->penetration = a->getRadius();

			// If the circles are at the exact same point,
			// we'll simply use the following normal vector
			m->normal = Vector2F(1, 0);
		}
		else
		{
			// Calculate the penetation and normal vector
			m->penetration = totalRadius - distance;

			// Normalize the vector from a to b (will be the collision normal)
			m->normal = normal / distance;
		}
	}

	void AABBToCircle(Manifold *m)
	{
		Box *a = dynamic_cast<Box*>(m->a);
		Circle *b = dynamic_cast<Circle*>(m->b);

		// Vector from a to b
		Vector2F delta = b->getCenter() - a->getCenter();

		// Calculate half extents of box
		Vector2F halfExtents = a->getSize() * 0.5f;

		// Closes point on a to the center of b
		Vector2F closest;
		closest.x = math::clamp(delta.x, -halfExtents.x, halfExtents.x);
		closest.y = math::clamp(delta.y, -halfExtents.y, halfExtents.y);

		// Delta did not change, meaning that the center of
		// the circle is inside the box
		bool inside = delta == closest;
		if(inside)
		{
			// TODO
		}

		Vector2F normal = delta - closest;
		float lengthSquared = normal.lengthSquared();
		float radius = b->getRadius();

		// Check if distance to the closest point is less than
		// the circle radius
		if(lengthSquared > radius * radius && !inside)
			return;

		float length = sqrt(lengthSquared);

		if(inside)
		{
			// TODO
		}
		else
		{
			m->normal = normal / length;
			m->penetration = radius - length;
		}
		m->contactCount += 1;
	}

	void CircleToAABB(Manifold *m)
	{
		m->swapShapes();
		AABBToCircle(m);
	}

	void AABBToAABB(Manifold *m)
	{
		Box *a = dynamic_cast<Box*>(m->a);
		Box *b = dynamic_cast<Box*>(m->b);

		// Calculate vector from a to b
		Vector2F d = b->getCenter() - a->getCenter();

		// Calculate half extents for each object
		Vector2F halfExtentsOfA = a->getSize() * 0.5f;
		Vector2F halfExtentsOfB = b->getSize() * 0.5f;

		// Calculate overlap on x-axis
		Vector2F overlaps = halfExtentsOfA + halfExtentsOfB - math::abs(d);

		// SAT test on x-axis
		if(overlaps.x > 0)
		{
			// SAT test on y-axis
			if(overlaps.y > 0)
			{
				// Find out which axis is axis of least penetration
				if(overlaps.x < overlaps.y)
				{
					// Create collision normal in the direction of B
					if(d.x < 0)
						m->normal = Vector2F(-1, 0);
					else
						m->normal = Vector2F(1, 0);
					m->penetration = overlaps.x;
				}
				else
				{
					// Create collision normal in the direction of B
					if(d.y < 0)
						m->normal = Vector2F(0, -1);
					else
						m->normal = Vector2F(0, 1);
					m->penetration = overlaps.y;
				}
				m->contactCount += 1;
				return;
			}
		}
	}
}

class PhysicsEngineGame : public Game
{
	vector<Shape*> shapes;
	Shape *selectedShape;
	Vector2F m_lastMousePosition;
	function<void(Manifold*)> m_manifoldGenerationFunctionTable[Shape::NUM_SHAPES][Shape::NUM_SHAPES];

public:
	void setupScene()
	{
		// Create test scene
		{
			Circle *circle1 = new Circle;
			circle1->setCenter(Vector2F(100.f, 100.f));
			circle1->setRadius(25.0f);
			shapes.push_back(circle1);

			Circle *circle2 = new Circle;
			circle2->setCenter(Vector2F(350.f, 200.f));
			circle2->setRadius(15.0f);
			shapes.push_back(circle2);

			Box *box1 = new Box;
			box1->setCenter(Vector2F(200.0f, 200.0f));
			box1->setSize(Vector2F(100.0f, 50.0f));
			shapes.push_back(box1);

			Box *box2 = new Box;
			box2->setCenter(Vector2F(400.0f, 400.0f));
			box2->setSize(Vector2F(150.0f, 100.0f));
			shapes.push_back(box2);

			Vector2I size = getWindow()->getSize();

			Box *ground = new Box;
			ground->setCenter(Vector2F(size.x * 0.5f, size.y - 10.0f));
			ground->setSize(Vector2F(size.x, 20.0f));
			ground->setMass(0.0f);
			shapes.push_back(ground);
		}

		selectedShape = nullptr;
	}

	void cleanScene()
	{
		for(Shape *shape : shapes)
		{
			delete shape;
		}
		shapes.clear();
		selectedShape = nullptr;
	}

	void onStart(GameEvent *e)
	{
		setupScene();

		m_manifoldGenerationFunctionTable[Shape::BOX][Shape::BOX] = manifolds::AABBToAABB;
		m_manifoldGenerationFunctionTable[Shape::CIRCLE][Shape::BOX] = manifolds::CircleToAABB;
		m_manifoldGenerationFunctionTable[Shape::BOX][Shape::CIRCLE] = manifolds::AABBToCircle;
		m_manifoldGenerationFunctionTable[Shape::CIRCLE][Shape::CIRCLE] = manifolds::CircleToCircle;

		Game::onStart(e);
	}

	void onEnd(GameEvent *e)
	{
		Game::onEnd(e);
	}

	void onTick(TickEvent *e)
	{
		// Apply velocity to selected shape
		if(selectedShape)
		{
			selectedShape->setVelocity((getInputManager()->getPosition() - selectedShape->getCenter()) * 0.25f / e->getDelta());
		}

		// Apply gravity to all shapes
		Vector2F gravity(0.0f, 150.0f * e->getDelta());
		for(Shape *shape : shapes)
		{
			if(shape->getMass() > 0)
			{
				shape->setVelocity(shape->getVelocity() + gravity);
			}
		}

		// Broadphase - Find and resolve all colliding shapes
		for(int i = 0; i < shapes.size(); i++)
		{
			bool colliding = false;
			Shape *shape = shapes[i];
			for(int j = i + 1; j < shapes.size(); j++)
			{
				Shape *otherShape = shapes[j];

				Manifold manifold(shape, otherShape);
				m_manifoldGenerationFunctionTable[shape->getType()][otherShape->getType()](&manifold);
				if(manifold.contactCount > 0)
				{
					ResolveCollision(&manifold);
					colliding = true;
				}
			}
			shape->m_isColliding = colliding;
			
			shape->move(shape->getVelocity() * e->getDelta());
		}
		Game::onTick(e);
	}

	void onDraw(DrawEvent *e)
	{
		for(Shape *shape : shapes)
		{
			//Color c = shape->colliding ? Color::Blue : Color::White;
			
			// Render shape
			Color color = Color::White;
			shape->draw(e->getGraphicsContext(), color);

			// Draw velocity arrow
			e->getGraphicsContext()->drawArrow(shape->getCenter(), shape->getCenter() + shape->getVelocity() * 0.1f, Color::Red);
		}
		Game::onDraw(e);
	}

	void ResolveCollision(Manifold *m)
	{
		// Get manifold shapes
		Shape *a = m->a;
		Shape *b = m->b;

		// Calculate relative velocity
		Vector2F relativeVel = b->getVelocity() - a->getVelocity();

		// Calculate relative velocity in terms of the normal direction
		float velAlongNormal = relativeVel.dot(m->normal);

		// Do not resolve if velocities are separating
		if(velAlongNormal >= 0.0f)
			return;

		// Calculate restitution
		float e = min(a->getRestitution(), b->getRestitution());

		// Calculate impulse scalar
		float j = (-(1 + e) * velAlongNormal) / (a->getMassInv() + b->getMassInv());

		// Apply impulse
		Vector2F impulse = m->normal * j;
		a->setVelocity(a->getVelocity() - impulse * a->getMassInv());
		b->setVelocity(b->getVelocity() + impulse * b->getMassInv());
	}

	void onMouseDown(MouseEvent *e)
	{
		Vector2F inputPos = e->getPosition();
		for(Shape *shape : shapes)
		{
			if(shape->contains(inputPos))
			{
				selectedShape = shape;
				break;
			}
		}
	}

	void onMouseUp(MouseEvent *e)
	{
		selectedShape = nullptr;
	}

	void onMouseMove(MouseEvent *e)
	{
		m_lastMousePosition = e->getPosition();
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