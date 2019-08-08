#include <Sauce/Sauce.h>
#include "Shapes.h"
#include "Manifold.h"

using namespace sauce;

// TODO:
// [x] Add static objects
// [-] Fix sinking
// [x] Verify that integration is implemented correctly (hint: delta time?)
// [ ] Should impulses be multiplied with delta (maybe not as an impulse is technically not the same as acceleration?)
// [x] Add gravity
// [x] Add a gravity scale variable
// [ ] Consider adding ImGui
// [x] Add friction
// [x] Add rotated boxes
// [ ] Add rotated circles
// [ ] Add rotated polygons

/*
	Rock       Density : 0.6  Restitution : 0.1
	Wood       Density : 0.3  Restitution : 0.2
	Metal      Density : 1.2  Restitution : 0.05
	BouncyBall Density : 0.3  Restitution : 0.8
	SuperBall  Density : 0.3  Restitution : 0.95
	Pillow     Density : 0.1  Restitution : 0.2
	Static     Density : 0.0  Restitution : 0.4
*/

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

	PhysicsPolygon::Vertex *GetFarthestPoint(PhysicsPolygon *polygon, Vector2F dir)
	{
		float maxDist = -FLT_MAX;
		int index = 0;
		for(int i = 0; i < 4; i++)
		{
			float dist = polygon->vertices[i]->position.dot(dir);
			if(dist > maxDist)
			{
				maxDist = dist;
				index = i;
			}
		}
		return polygon->vertices[index];
	}

	vector<Vector2F> ClipEdge(Vector2F v0, Vector2F v1, Vector2F n, float o)
	{
		vector<Vector2F> clipEdges;
		clipEdges.reserve(2);

		float d0 = n.dot(v0) - o;
		float d1 = n.dot(v1) - o;

		if(d0 >= 0.0f) clipEdges.push_back(v0);
		if(d1 >= 0.0f) clipEdges.push_back(v1);

		if(d0 * d1 < 0.0f)
		{
			float u = d0 / (d0 - d1);
			Vector2F e = (v1 - v0) * u + v0;
			clipEdges.push_back(e);
		}

		return clipEdges;
	}

	void AABBToOBB(Manifold *m)
	{
		/*
		SAT:
		In order to confirm a collision, overlapping on all axes has to be true -- if there's any axis without an overlap, we can conclude that there's no collision.
		4 normals/directions/axes to check, 2 per box
		*/
		Box *a = dynamic_cast<Box*>(m->a);
		Box *b = dynamic_cast<Box*>(m->b);

		a->debugPoints.clear();
		b->debugPoints.clear();

		// Calculate vector from a to b
		Vector2F deltaPositions = b->getCenter() - a->getCenter();

		// TODO: Fix memory leak here
		PhysicsPolygon polygonA;
		PhysicsPolygon polygonB;
		a->getPolygon(&polygonA);
		b->getPolygon(&polygonB);

		Vector2F axes[4];
		axes[0] = polygonA.edges[0]->normal.dot(deltaPositions) > 0.0f ? polygonA.edges[0]->normal : polygonA.edges[2]->normal;
		axes[1] = polygonA.edges[1]->normal.dot(deltaPositions) > 0.0f ? polygonA.edges[1]->normal : polygonA.edges[3]->normal;
		axes[2] = polygonB.edges[0]->normal.dot(deltaPositions) > 0.0f ? polygonB.edges[0]->normal : polygonB.edges[2]->normal;
		axes[3] = polygonB.edges[1]->normal.dot(deltaPositions) > 0.0f ? polygonB.edges[1]->normal : polygonB.edges[3]->normal;

		// For each axis
		float minPenetration = FLT_MAX; int minPenetrationAxis = -1;
		for(int i = 0; i < 4; i++)
		{
			Vector2F axis = axes[i];

			// Find min and max extents when projected onto the axis
			float halfWidthOfA;
			{
				float dotMin = FLT_MAX;
				float dotMax = -FLT_MAX;
				for(int j = 0; j < 4; j++)
				{
					float dot = axis.dot(polygonA.vertices[j]->localPosition);
					if(dot > dotMax)
					{
						dotMax = dot;
					}
					if(dot < dotMin)
					{
						dotMin = dot;
					}
				}
				halfWidthOfA = (dotMax - dotMin) * 0.5f;
			}

			float halfWidthOfB;
			{
				float dotMin = FLT_MAX;
				float dotMax = -FLT_MAX;
				for(int j = 0; j < 4; j++)
				{
					float dot = axis.dot(polygonB.vertices[j]->localPosition);
					if(dot > dotMax)
					{
						dotMax = dot;
					}
					if(dot < dotMin)
					{
						dotMin = dot;
					}
				}
				halfWidthOfB = (dotMax - dotMin) * 0.5f;
			}

			float projectedDistance = abs(axis.dot(deltaPositions)); // Find distance between A and B along the axis
			float penetration = halfWidthOfA + halfWidthOfB - projectedDistance;

			// SAT: Return if no overlap along this axis
			if(penetration < 0.0f)
				return;

			if(penetration < minPenetration)
			{
				minPenetration = penetration;
				minPenetrationAxis = i;
			}
		}

		m->normal = axes[minPenetrationAxis];

		// Find the vertex farthest along -n for object b
		PhysicsPolygon::Vertex *farthestCornerOfB = GetFarthestPoint(&polygonB, -m->normal);

		// Find the vertex farthest along +n for object a
		PhysicsPolygon::Vertex *farthestCornerOfA = GetFarthestPoint(&polygonA, m->normal);

		// Determine best edge of a and b by considering the edges
		// neighbouring the fartest vertices, and picking the ones
		// that are most perpendicular to the collision normal

		// Select edge that is most perpendicular to separation normal
		// out of the two neighbouring edges of the farthest vertex of A
		PhysicsPolygon::Edge *bestEdgeOfA =
			abs(farthestCornerOfA->leftEdge->normal.dot(m->normal)) < abs(farthestCornerOfA->rightEdge->normal.dot(m->normal)) ?
			farthestCornerOfA->leftEdge : farthestCornerOfA->rightEdge;

		// Repeat for B
		PhysicsPolygon::Edge *bestEdgeOfB =
			abs(farthestCornerOfB->leftEdge->normal.dot(m->normal)) < abs(farthestCornerOfB->rightEdge->normal.dot(m->normal)) ?
			farthestCornerOfB->leftEdge : farthestCornerOfB->rightEdge;

		// Determine reference and incident edge
		// The reference edge is the edge most
		// perpendicular to the separation normal
		PhysicsPolygon::Edge *referenceEdge, *incidentEdge;
		bool flip = false;
		if(abs(bestEdgeOfA->normal.dot(m->normal)) < abs(bestEdgeOfB->normal.dot(m->normal)))
		{
			referenceEdge = bestEdgeOfA;
			incidentEdge = bestEdgeOfB;
		}
		else
		{
			referenceEdge = bestEdgeOfB;
			incidentEdge = bestEdgeOfA;
			flip = true;
		}

		Vector2F referenceEdgeVector = Vector2F(-referenceEdge->normal.y, referenceEdge->normal.x);

		float o = referenceEdgeVector.dot(referenceEdge->v0->position);
		vector<Vector2F> clippedEdge = ClipEdge(incidentEdge->v0->position, incidentEdge->v1->position, referenceEdgeVector, o);
		if(clippedEdge.size() < 2)
			return;

		o = referenceEdgeVector.dot(referenceEdge->v1->position);
		clippedEdge = ClipEdge(clippedEdge[0], clippedEdge[1], -referenceEdgeVector, -o);
		if(clippedEdge.size() < 2)
			return;

		//a->debugPoints.push_back(make_pair(clippedEdge[0], Color(0, 255, 0)));
		//a->debugPoints.push_back(make_pair(clippedEdge[1], Color(0, 255, 0)));

		Vector2F referenceEdgeNormal = referenceEdge->normal;//flip ? -referenceEdge->normal : referenceEdge->normal;
		if(referenceEdgeNormal.dot(clippedEdge[0] - referenceEdge->v0->position) < 0.0f)
		{
			m->contactPoints.push_back(clippedEdge[0]);
		}

		if(referenceEdgeNormal.dot(clippedEdge[1] - referenceEdge->v0->position) < 0.0f)
		{
			m->contactPoints.push_back(clippedEdge[1]);
		}

		m->penetration = minPenetration;
		m->contactCount += 1;
	}
}

class PhysicsEngineGame : public Game
{
	vector<Shape*> shapes;
	Shape *selectedShape;
	Vector2F m_lastMousePosition;
	function<void(Manifold*)> m_manifoldGenerationFunctionTable[Shape::NUM_SHAPES][Shape::NUM_SHAPES];

	list<Manifold> m_manifolds;

	Vector2F gravity = Vector2F(0.0f, 150.0f);

public:
	void setupScene()
	{
		// Create test scene
		{
			/*Circle *circle1 = new Circle;
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
			*/
			Vector2I size = getWindow()->getSize();

			Box *ground = new Box;
			ground->setCenter(Vector2F(size.x * 0.5f, size.y - 10.0f));
			ground->setSize(Vector2F(size.x, 20.0f));
			ground->setMass(0.0f);
			ground->setInertia(0.0f);
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
		
		getWindow()->getGraphicsContext()->setPointSize(5.0f);

		m_manifoldGenerationFunctionTable[Shape::BOX][Shape::BOX] = manifolds::AABBToOBB;//manifolds::AABBToAABB;
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
		const float timeScale = 1.0f;
		const float dt = e->getDelta() * timeScale;

		// Apply velocity to selected shape
		if(selectedShape)
		{
			selectedShape->setVelocity((getInputManager()->getPosition() - selectedShape->getCenter()) * 0.25f / e->getDelta());
		}

		// Apply gravity to all shapes
		for(Shape *shape : shapes)
		{
			if(!shape->isStatic())
			{
				shape->setVelocity(shape->getVelocity() + gravity * dt);
			}
			shape->m_isColliding = false;
		}

		m_manifolds.clear();

		// Broadphase - Find and resolve all colliding shapes
		for(int i = 0; i < shapes.size(); i++)
		{
			Shape *shape = shapes[i];
			for(int j = i + 1; j < shapes.size(); j++)
			{
				Shape *otherShape = shapes[j];

				// Skip collision between static objects
				if(shape->isStatic() && otherShape->isStatic())
					continue;

				Manifold manifold(shape, otherShape);
				m_manifoldGenerationFunctionTable[shape->getType()][otherShape->getType()](&manifold);
				if(manifold.contactCount > 0)
				{
					ResolveCollision(&manifold);
					CorrectPositions(&manifold);
					m_manifolds.push_back(manifold);
					shape->m_isColliding = otherShape->m_isColliding = true;
				}
			}
		}

		for(Shape *shape : shapes)
		{
			shape->move(shape->getVelocity() * dt);
			shape->rotate(shape->getAngularVelocity() * dt);
		}

		Game::onTick(e);
	}

	void onDraw(DrawEvent *e)
	{
		for(Shape *shape : shapes)
		{
			Color color = shape->m_isColliding ? Color::Blue : Color::White;
			
			// Render shape
			//Color color = Color::White;
			shape->draw(e->getGraphicsContext(), color);

			// Draw velocity arrow
			e->getGraphicsContext()->drawArrow(shape->getCenter(), shape->getCenter() + shape->getVelocity() * 0.1f, Color::Red);
		}

		for(Manifold &m : m_manifolds)
		{
			for(Vector2F contactPoint : m.contactPoints)
			{
				Vertex v;
				v.set2f(VERTEX_POSITION, contactPoint.x, contactPoint.y);
				v.set4ub(VERTEX_COLOR, 255, 255, 0, 255);
				e->getGraphicsContext()->drawPrimitives(GraphicsContext::PRIMITIVE_POINTS, &v, 1);
				e->getGraphicsContext()->drawArrow(contactPoint, contactPoint + m.normal * 15.0f, Color::Red);
			}
		}

		Game::onDraw(e);
	}

	Vector2F perp(const Vector2F &v)
	{
		return Vector2F(-v.y, v.x);
	}

	Vector2F getRelativeVelocity(Shape *a, Shape *b, Vector2F rAP, Vector2F rBP)
	{
		//return b->getVelocity() + perp(rBP) * b->getAngularVelocity() -
		//	a->getVelocity() - perp(rAP) * a->getAngularVelocity();
		return b->getVelocity() - a->getVelocity();
	}

	void ResolveCollision(Manifold *m)
	{
		float numContacts = m->contactPoints.size();
		for(Vector2F contactPoint : m->contactPoints)
		{
			// Get manifold shapes
			Shape *a = m->a;
			Shape *b = m->b;

			Vector2F rAP = contactPoint - a->getCenter();
			Vector2F rBP = contactPoint - b->getCenter();

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
			j /= numContacts;

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
			jt /= numContacts;

			// Approximate mu given friction coefficients of each body
			float mu = std::sqrt(a->staticFriction * a->staticFriction +
								 b->staticFriction * b->staticFriction);

			// Clamp magnitude of friction and calculate impulse vector
			Vector2F frictionImpulse;
			if(abs(jt) < j * mu)
			{
				frictionImpulse = tangent * jt;
			}
			else
			{
				mu = std::sqrt(a->dynamicFriction * a->dynamicFriction +
							   b->dynamicFriction * b->dynamicFriction);
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
		Shape *a = m->a;
		Shape *b = m->b;

		// Correct the positions
		const float slop = 0.01;    // Usually 0.01 to 0.1
		const float percent = 0.5f; // Usually 20% to 80%
		Vector2F correction = m->normal * percent * max(m->penetration - slop, 0.0f) / (a->getMassInv() + b->getMassInv());
		a->setCenter(a->getCenter() - correction * a->getMassInv());
		b->setCenter(b->getCenter() + correction * b->getMassInv());
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

	void onMouseWheel(MouseEvent *e)
	{
		if(selectedShape)
		{
			const float rotationalSpeed = 5.0f;
			selectedShape->m_angle += e->getWheelY() * rotationalSpeed;
		}		
	}

	void onKeyDown(KeyEvent *e)
	{
		switch(e->getKeycode())
		{
			case Keycode::SAUCE_KEY_1:
			{
				Circle *circle = new Circle;
				circle->setCenter(e->getInputManager()->getPosition());
				circle->setRadius(25.0f);
				if(e->getModifiers() & KeyEvent::SHIFT)
				{
					circle->setMass(0.0f);
				}
				shapes.push_back(circle);
			}
			break;

			case Keycode::SAUCE_KEY_2:
			{
				Box *box = new Box;
				box->setCenter(e->getInputManager()->getPosition());
				box->setSize(Vector2F(50.0f, 50.0f));
				if(e->getModifiers() & KeyEvent::SHIFT)
				{
					box->setMass(0.0f);
				}
				shapes.push_back(box);
			}
			break;

			case Keycode::SAUCE_KEY_R:
			{
				cleanScene();
				setupScene();
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