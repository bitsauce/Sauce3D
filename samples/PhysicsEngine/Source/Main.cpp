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

class PhysicsEngineGame : public Game
{
	vector<Shape*> shapes;
	Shape *selectedShape;
	Vector2F m_lastMousePosition;

public:
	void onStart(GameEvent *e)
	{
		selectedShape = nullptr;

		// Create test scene
		{
			Circle *circle1 = new Circle;
			circle1->position.set(100.f, 100.f);
			circle1->radius = 25.0f;
			shapes.push_back(circle1);

			Circle *circle2 = new Circle;
			circle2->position.set(350.f, 200.f);
			circle2->radius = 15.0f;
			shapes.push_back(circle2);

			Box *box1 = new Box;
			box1->min.set(200.0f, 200.0f);
			box1->max.set(300.0f, 250.0f);
			shapes.push_back(box1);

			Box *box2 = new Box;
			box2->min.set(400.0f, 400.0f);
			box2->max.set(550.0f, 500.0f);
			shapes.push_back(box2);

			Vector2I size = getWindow()->getSize();

			Box *ground = new Box;
			ground->min.set(0.0f, size.y - 20);
			ground->max.set(size.x, size.y);
			ground->mass = 0.0f;
			shapes.push_back(ground);
		}

		Game::onStart(e);
	}

	void onEnd(GameEvent *e)
	{
		Game::onEnd(e);
	}

	void onMouseDown(MouseEvent *e)
	{
		Vector2F inputPos = e->getPosition();
		for(Shape *shape : shapes)
		{
			switch(shape->getType())
			{
				case Shape::BOX:
				{
					Box *aabb = dynamic_cast<Box*>(shape);
					if(inputPos.x >= aabb->min.x && inputPos.x <= aabb->max.x && inputPos.y >= aabb->min.y && inputPos.y <= aabb->max.y)
					{
						selectedShape = shape;
					}
				}
				break;

				case Shape::CIRCLE:
				{
					Circle *circle = dynamic_cast<Circle*>(shape);
					if((circle->position - inputPos).lengthSquared() < circle->radius * circle->radius)
					{
						selectedShape = shape;
					}
				}
				break;
			}
		}
	}

	void onMouseUp(MouseEvent *e)
	{
		selectedShape = 0;
	}

	void onMouseMove(MouseEvent *e)
	{
		m_lastMousePosition = e->getPosition();
	}

	void onTick(TickEvent *e)
	{
		// Mouse drag movement
		if(selectedShape)
		{
			switch(selectedShape->getType())
			{
			case Shape::BOX:
			{
				Box *aabb = dynamic_cast<Box*>(selectedShape);
				aabb->velocity = (getInputManager()->getPosition() - aabb->getCentroid()) * 0.25f;
			}
			break;

			case Shape::CIRCLE:
			{
				Circle *circle = dynamic_cast<Circle*>(selectedShape);
				circle->velocity = (getInputManager()->getPosition() - circle->position) * 0.25f;
			}
			break;
			}
		}

		const float gravity = 1.0f;
		for(Shape *shape : shapes)
		{
			if (shape->mass > 0)
				shape->velocity.y += gravity;
		}

		for(Shape *shape : shapes)
		{
			bool colliding = false;
			for(Shape *otherShape : shapes)
			{
				if(shape == otherShape) continue;

				// Circle to circle collision
				if(shape->getType() == Shape::CIRCLE && otherShape->getType() == Shape::CIRCLE)
				{
					Manifold manifold(shape, otherShape);
					CircleToCircle(&manifold);
					if(manifold.contactCount > 0)
					{
						ResolveCollision(&manifold, shape, otherShape);
						colliding = true;
						break;
					}
				}
				// Box to circle collision
				else if(shape->getType() == Shape::BOX && otherShape->getType() == Shape::CIRCLE)
				{
					Manifold manifold(shape, otherShape);
					if(AABBToCircle(&manifold))
					{
						ResolveCollision(&manifold, shape, otherShape);
						colliding = true;
						break;
					}
				}
				// Circle to box collision
				else if(shape->getType() == Shape::CIRCLE && otherShape->getType() == Shape::BOX)
				{
					Manifold manifold(otherShape, shape);
					if(AABBToCircle(&manifold))
					{
						ResolveCollision(&manifold, otherShape, shape);
						colliding = true;
						break;
					}
				}
				// Box to box collision
				else if(shape->getType() == Shape::BOX && otherShape->getType() == Shape::BOX)
				{
					Manifold manifold(shape, otherShape);
					AABBToAABB(&manifold);
					if(manifold.contactCount > 0)
					{
						ResolveCollision(&manifold, shape, otherShape);
						colliding = true;
						break;
					}
				}
			}
			shape->colliding = colliding;
			
			switch(shape->getType())
			{
			case Shape::BOX:
			{
				Box *aabb = dynamic_cast<Box*>(shape);
				aabb->min += aabb->velocity;
				aabb->max += aabb->velocity;
			}
			break;

			case Shape::CIRCLE:
			{
				Circle *circle = dynamic_cast<Circle*>(shape);
				circle->position += circle->velocity;
			}
			break;
			}
		}
		Game::onTick(e);
	}

	void onDraw(DrawEvent *e)
	{
		for(Shape *shape : shapes)
		{
			//Color c = shape->colliding ? Color::Blue : Color::White;
			Color c = Color::White;
			switch(shape->getType())
			{
				case Shape::BOX:
				{
					Box *aabb = dynamic_cast<Box*>(shape);
					e->getGraphicsContext()->drawRectangle(aabb->min, aabb->max - aabb->min, c);
				}
				break;

				case Shape::CIRCLE:
				{
					Circle *circle = dynamic_cast<Circle*>(shape);
					e->getGraphicsContext()->drawCircle(circle->position, circle->radius, 32, c);
				}
				break;
			}
			e->getGraphicsContext()->drawArrow(shape->getCentroid(), shape->getCentroid() + shape->velocity, Color::Red);
		}
		Game::onDraw(e);
	}

	void ResolveCollision(Manifold *m, Shape *a, Shape *b)
	{
		// Calculate relative velocity
		Vector2F relativeVel = b->velocity - a->velocity;

		// Calculate relative velocity in terms of the normal direction
		float velAlongNormal = relativeVel.dot(m->normal);

		// Do not resolve if velocities are separating
		if(velAlongNormal > 0)
			return;

		// Calculate restitution
		float e = min(a->restitution, b->restitution);

		// Calculate impulse scalar
		float j = -(1 + e) * velAlongNormal;

		float massAInv = a->mass > 0.0f ? 1 / a->mass : 0.0f;
		float massBInv = b->mass > 0.0f ? 1 / b->mass : 0.0f;

		j /= massAInv + massBInv;

		// Apply impulse
		Vector2F impulse = m->normal * j;
		a->velocity -= impulse * massAInv;
		b->velocity += impulse * massBInv;
	}

	void CircleToCircle(Manifold *m)
	{
		Circle *a = dynamic_cast<Circle*>(m->a);
		Circle *b = dynamic_cast<Circle*>(m->b);

		// Calculate vector from a to b
		Vector2F normal = b->position - a->position;
		float lengthSquared = normal.lengthSquared();
		float radius = a->radius + b->radius;

		// If their combined radius is less than the distance between,
		// there is no contact
		if(lengthSquared >= radius * radius)
		{
			m->contactCount = 0;
			return;
		}

		// There is contact, calculate the distance using sqrt
		float distance = std::sqrt(lengthSquared);
		m->contactCount = 1;
		if(distance == 0.0f)
		{
			// If the circles are at the exact same point,
			// just pick a pre-determined normal vector
			m->penetration = a->radius;
			m->normal = Vector2F(1, 0);
		}
		else
		{
			// Calculate the penetation and normal vector
			m->penetration = radius - distance;
			m->normal = normal / distance; // Normalize the vector from a to b
			                               // (will be the collision normal)
		}
	}

	bool AABBToCircle(Manifold *m)
	{
		Box *a = dynamic_cast<Box*>(m->a);
		Circle *b = dynamic_cast<Circle*>(m->b);

		// Vector from a to b
		Vector2F delta = b->getCentroid() - a->getCentroid();

		// Calculate half extents of box
		float halfExtentX = (a->max.x - a->min.x) * 0.5f;
		float halfExtentY = (a->max.y - a->min.y) * 0.5f;

		// Closes point on a to the center of b
		Vector2F closest;
		closest.x = math::clamp(delta.x, -halfExtentX, halfExtentX);
		closest.y = math::clamp(delta.y, -halfExtentY, halfExtentY);

		// Delta did not change, meaning that the center of
		// the circle is inside the box
		bool inside = delta == closest;
		if(inside)
		{
			// TODO
		}

		Vector2F normal = delta - closest;
		float lengthSquared = normal.lengthSquared();
		float radius = b->radius;

		// Check if distance to the closest point is less than
		// the circle radius
		m->contactCount = 0;
		if(lengthSquared > radius * radius && !inside)
			return false;

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
		m->contactCount = 1;
		return true;
	}

	bool AABBToAABB(Manifold *m)
	{
		Box *a = dynamic_cast<Box*>(m->a);
		Box *b = dynamic_cast<Box*>(m->b);

		// Calculate vector from a to b
		Vector2F d = b->getCentroid() - a->getCentroid();

		// Calculate half extents along x-axis for each object
		float widthOfA = a->max.x - a->min.x;
		float widthOfB = b->max.x - b->min.x;

		// Calculate overlap on x-axis
		float overlapX = (widthOfA + widthOfB) * 0.5f - abs(d.x);

		// SAT test on x-axis
		if(overlapX > 0)
		{
			// Calculate half extents along y-axis for each object
			float heightOfA = a->max.y - a->min.y;
			float heightOfB = b->max.y - b->min.y;

			// Calculate overlap on y-axis
			float overlapY = (heightOfA + heightOfB) * 0.5f - abs(d.y);

			// SAT test on y-axis
			if(overlapY > 0)
			{
				// Find out which axis is axis of least penetration
				if(overlapX < overlapY)
				{
					// Create collision normal in the direction of B
					if(d.x < 0)
						m->normal = Vector2F(-1, 0);
					else
						m->normal = Vector2F(1, 0);
					m->penetration = overlapX;
					m->contactCount = 1;
					return true;
				}
				else
				{
					// Create collision normal in the direction of B
					if(d.y < 0)
						m->normal = Vector2F(0, -1);
					else
						m->normal = Vector2F(0, 1);
					m->penetration = overlapY;
					m->contactCount = 1;
					return true;
				}
			}
		}

		// Exit with no intersection if found separated along an axis
		m->contactCount = 0;
		return false;
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