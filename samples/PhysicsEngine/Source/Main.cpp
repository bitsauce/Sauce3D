#include <Sauce/Sauce.h>
#include "Body.h"
#include "Shapes.h"
#include "Manifold.h"

using namespace sauce;

#define DEBUG_DRAW

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
// [ ] Implement "mass = volume * density" mass initialization
// [ ] Fix drifting with stacked boxes
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

namespace manifolds
{
	void CircleToCircle(Manifold *m)
	{
		Circle *a = dynamic_cast<Circle*>(m->shapeA);
		Circle *b = dynamic_cast<Circle*>(m->shapeB);

		// Rotation only matrices (used later)
		Matrix4 worldToBodyARotationOnly;
		Matrix4 bodyAToWorldRotationOnly;
		Matrix4 bodyBToWorldRotationOnly;

		// Get body to world matrices
		Matrix4 bodyAToWorld = m->bodyA->bodyLocalToWorld(&bodyAToWorldRotationOnly);
		Matrix4 bodyBToWorld = m->bodyB->bodyLocalToWorld(&bodyBToWorldRotationOnly);

		// Get matrices for transforming from bodyB space to bodyA space
		Matrix4 bodyBToBodyA = m->bodyA->worldToBodyLocal(&worldToBodyARotationOnly) * bodyBToWorld;
		Matrix4 bodyBToBodyARotationOnly = worldToBodyARotationOnly * bodyBToWorldRotationOnly;

		// Calculate vector from shape in bodyA to shape in bodyB
		const Vector2F normal = bodyBToBodyA * b->getLocalPosition() - a->getLocalPosition();
		const float lengthSquared = normal.lengthSquared();
		const float totalRadius = a->getRadius() + b->getRadius();

		// If their combined radius is less than the distance between,
		// there is no contact
		if(lengthSquared >= totalRadius * totalRadius)
		{
			return;
		}

		// There is contact, calculate the distance using sqrt
		float distance = std::sqrt(lengthSquared);
		if(distance == 0.0f)
		{
			m->penetration = a->getRadius();
			m->contactPoints.push_back(m->bodyA->getPosition());

			// If the circles are at the exact same point,
			// we'll simply use the following normal vector
			m->normal = Vector2F(1, 0);
;		}
		else
		{
			// Calculate the penetation and normal vector
			m->penetration = totalRadius - distance;

			// Normalize the vector from a to b (will be the collision normal)
			m->normal = normal / distance;

			// Calculate contact point
			m->contactPoints.push_back(bodyAToWorld * (m->normal * a->getRadius() + m->shapeA->getLocalPosition()));
		}

		// Transform collision normal to world space
		m->normal = bodyAToWorldRotationOnly * m->normal;
	}

	void PolygonToCircle(Manifold *m)
	{
		PolygonShape *a = dynamic_cast<PolygonShape*>(m->shapeA);
		Circle *b = dynamic_cast<Circle*>(m->shapeB);

		// Rotation only matrices (used later)
		Matrix4 worldToBodyARotationOnly;
		Matrix4 bodyAToWorldRotationOnly;
		Matrix4 bodyBToWorldRotationOnly;

		// Get body to world matrices
		Matrix4 bodyAToWorld = m->bodyA->bodyLocalToWorld(&bodyAToWorldRotationOnly);
		Matrix4 bodyBToWorld = m->bodyB->bodyLocalToWorld(&bodyBToWorldRotationOnly);

		// Get matrices for transforming from bodyB space to bodyA space
		Matrix4 bodyBToBodyA = m->bodyA->worldToBodyLocal(&worldToBodyARotationOnly) * bodyBToWorld;
		Matrix4 bodyBToBodyARotationOnly = worldToBodyARotationOnly * bodyBToWorldRotationOnly;

		// Calculate vector from shape in bodyA to shape in bodyB
		const Vector2F shapePositionB = bodyBToBodyA * b->getLocalPosition();
		const float radiusOfB = b->getRadius();
		const Vector2F deltaPosition = shapePositionB - a->getLocalPosition();

		PhysicsPolygon &polygonA = *a->getPolygon();
		float separation = -FLT_MAX;
		int faceNormal = 0;
		for(int i = 0; i < polygonA.vertices.size(); i++)
		{
			float s = polygonA.edges[i]->normal.dot(shapePositionB - polygonA.vertices[i]->position);
			if(s > radiusOfB)
			{
				return;
			}

			if(s > separation)
			{
				separation = s;
				faceNormal = i;
			}
		}

		PhysicsPolygon::Edge *edge = polygonA.edges[faceNormal];

		if(separation < 0.0f)
		{
			m->normal = edge->normal;
			m->contactPoints.push_back(bodyAToWorld * (m->normal * radiusOfB + bodyBToBodyA * b->getLocalPosition()));
			m->normal = bodyAToWorldRotationOnly * m->normal;
			m->penetration = radiusOfB;
			return;
		}

		float dot1 = (shapePositionB - edge->v0->position).dot(edge->v1->position - edge->v0->position);
		float dot2 = (shapePositionB - edge->v1->position).dot(edge->v0->position - edge->v1->position);
		if(dot1 <= 0.0f)
		{
			if((shapePositionB - edge->v0->position).lengthSquared() > radiusOfB * radiusOfB)
			{
				return;
			}

			m->normal = -(edge->v0->position - shapePositionB).normalized();
			m->contactPoints.push_back(bodyAToWorld * (edge->v0->position + a->getLocalPosition()));
		}
		else if(dot2 <= 0.0f)
		{
			if((shapePositionB - edge->v1->position).lengthSquared() > radiusOfB * radiusOfB)
			{
				return;
			}

			m->normal = -(edge->v1->position - shapePositionB).normalized();
			m->contactPoints.push_back(bodyAToWorld * (edge->v1->position + a->getLocalPosition()));
		}
		else
		{
			if(edge->normal.dot(shapePositionB - edge->v0->position) > radiusOfB)
			{
				return;
			}

			m->normal = edge->normal;
			m->contactPoints.push_back(bodyAToWorld * (-m->normal * radiusOfB + bodyBToBodyA * b->getLocalPosition()));
		}
		m->normal = bodyAToWorldRotationOnly * m->normal;
	}

	void CircleToPolygon(Manifold *m)
	{
		m->swapAAndB();
		PolygonToCircle(m);
	}

	PhysicsPolygon::Vertex *GetFarthestPoint(PhysicsPolygon *polygon, Vector2F dir)
	{
		float maxDist = -FLT_MAX;
		int index = 0;
		for(int i = 0; i < polygon->vertices.size(); i++)
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

	void PolygonToPolygon(Manifold *m)
	{
		PolygonShape *a = dynamic_cast<PolygonShape*>(m->shapeA);
		PolygonShape *b = dynamic_cast<PolygonShape*>(m->shapeB);

		// Rotation only matrices (used later)
		Matrix4 worldToBodyARotationOnly;
		Matrix4 bodyAToWorldRotationOnly;
		Matrix4 bodyBToWorldRotationOnly;

		// Get body to world matrices
		Matrix4 bodyAToWorld = m->bodyA->bodyLocalToWorld(&bodyAToWorldRotationOnly);
		Matrix4 bodyBToWorld = m->bodyB->bodyLocalToWorld(&bodyBToWorldRotationOnly);

		// Get matrices for transforming from bodyB space to bodyA space
		Matrix4 bodyBToBodyA = m->bodyA->worldToBodyLocal(&worldToBodyARotationOnly) * bodyBToWorld;
		Matrix4 bodyBToBodyARotationOnly = worldToBodyARotationOnly * bodyBToWorldRotationOnly;

		// Calculate vector from shape in bodyA to shape in bodyB
		Vector2F deltaPositions = bodyBToBodyA * b->getLocalPosition() - a->getLocalPosition();

		PhysicsPolygon &polygonA = *a->getPolygon();
		PhysicsPolygon &polygonB = *b->getPolygon(bodyBToBodyARotationOnly, bodyBToBodyA);

		vector<Vector2F> axes;
		for(PhysicsPolygon::Edge *edge : polygonA.edges)
		{
			// TODO: Does this actually work?
			const Vector2F axis = edge->normal.dot(deltaPositions) > 0.0f ? edge->normal : -edge->normal;
			axes.push_back(axis); // TODO: Verify that the same axis is not added twice
		}

		for(PhysicsPolygon::Edge *edge : polygonB.edges)
		{
			const Vector2F axis = edge->normal.dot(deltaPositions) > 0.0f ? edge->normal : -edge->normal;
			axes.push_back(axis);
		}

		// Determine if the shapes are overlapping by iterating every axis of both shapes
		// and checking the separation along every axis (SAT)
		//
		// Separating Axis Teorem (SAT):
		// In order to confirm a collision, overlapping on all axes has to be true -- 
		// if there's any axis without an overlap, we can conclude that there's no collision.
		//
		// For oriented boxes, there are 4 axes (normals) to check in total, 2 per box
		float minPenetration = FLT_MAX; int minPenetrationAxis = -1;
		for(int i = 0; i < axes.size(); i++)
		{
			const Vector2F axis = axes[i];

			// Find min and max extents when projected onto the axis
			// TODO: Optimizations may be done here since we are already using
			//       body A's space as reference
			float dotMinA = FLT_MAX;
			float dotMaxA = -FLT_MAX;
			{
				for(int j = 0; j < polygonA.vertices.size(); j++)
				{
					float dot = axis.dot(polygonA.vertices[j]->position);
					if(dot > dotMaxA)
					{
						dotMaxA = dot;
					}
					if(dot < dotMinA)
					{
						dotMinA = dot;
					}
				}
			}

			float dotMinB = FLT_MAX;
			float dotMaxB = -FLT_MAX;
			{
				for(int j = 0; j < polygonB.vertices.size(); j++)
				{
					float dot = axis.dot(polygonB.vertices[j]->position);
					if(dot > dotMaxB)
					{
						dotMaxB = dot;
					}
					if(dot < dotMinB)
					{
						dotMinB = dot;
					}
				}
			}

			float penetration;
			if(dotMaxA > dotMaxB)
			{
				penetration = dotMaxB - dotMinA;
			}
			else
			{
				penetration = dotMaxA - dotMinB;
			}

			// SAT: Return if no overlap along this axis
			if(penetration < 0.0f)
				return;

			if(penetration < minPenetration)
			{
				minPenetration = penetration;
				minPenetrationAxis = i;
			}
		}

		// Store axis of least penetation
		m->normal = axes[minPenetrationAxis];
		m->penetration = minPenetration;

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
			abs(farthestCornerOfA->leftEdge->normal.dot(m->normal)) > abs(farthestCornerOfA->rightEdge->normal.dot(m->normal)) ?
			farthestCornerOfA->leftEdge : farthestCornerOfA->rightEdge;

		// Repeat for B
		PhysicsPolygon::Edge *bestEdgeOfB =
			abs(farthestCornerOfB->leftEdge->normal.dot(m->normal)) > abs(farthestCornerOfB->rightEdge->normal.dot(m->normal)) ?
			farthestCornerOfB->leftEdge : farthestCornerOfB->rightEdge;

		// Determine reference and incident edge
		// The reference edge is the edge most
		// perpendicular to the separation normal
		PhysicsPolygon::Edge *referenceEdge, *incidentEdge;
		if(abs(bestEdgeOfA->normal.dot(m->normal)) < abs(bestEdgeOfB->normal.dot(m->normal)))
		{
			referenceEdge = bestEdgeOfA;
			incidentEdge = bestEdgeOfB;
		}
		else
		{
			referenceEdge = bestEdgeOfB;
			incidentEdge = bestEdgeOfA;
		}

		// Perform edge clipping
		Vector2F referenceEdgeVector = Vector2F(-referenceEdge->normal.y, referenceEdge->normal.x);

		float o = referenceEdgeVector.dot(referenceEdge->v0->position);
		vector<Vector2F> clippedEdge = ClipEdge(incidentEdge->v0->position, incidentEdge->v1->position, referenceEdgeVector, o);
		if(clippedEdge.size() < 2)
			return;

		o = referenceEdgeVector.dot(referenceEdge->v1->position);
		clippedEdge = ClipEdge(clippedEdge[0], clippedEdge[1], -referenceEdgeVector, -o);
		if(clippedEdge.size() < 2)
			return;

		Vector2F referenceEdgeNormal = referenceEdge->normal;
		if(referenceEdgeNormal.dot(clippedEdge[0] - referenceEdge->v0->position) < 0.0f)
		{
			m->contactPoints.push_back(bodyAToWorld * clippedEdge[0]);
		}

		if(referenceEdgeNormal.dot(clippedEdge[1] - referenceEdge->v0->position) < 0.0f)
		{
			m->contactPoints.push_back(bodyAToWorld * clippedEdge[1]);
		}

		// Transform collision normal to world space
		m->normal = bodyAToWorldRotationOnly * m->normal;

#ifdef DEBUG_DRAW
		Vector2F edge = referenceEdge->v1->position - referenceEdge->v0->position;
		for(float f = 0.0f; f < 1.0f; f += 0.1f)
		{
			a->debugPoints.push_back(make_pair(referenceEdge->v0->position + edge * f, Color(0, 255, 0)));
		}

		edge = incidentEdge->v1->position - incidentEdge->v0->position;
		for(float f = 0.0f; f < 1.0f; f += 0.1f)
		{
			a->debugPoints.push_back(make_pair(incidentEdge->v0->position + edge * f, Color(255, 0, 0)));
		}

		//a->debugPoints.push_back(make_pair(farthestCornerOfB->position, Color(0, 255, 0)));
		//a->debugPoints.push_back(make_pair(farthestCornerOfA->position, Color(0, 255, 0)));
		//if(minPenetrationAxis < 2)
		//	a->debugPoints.push_back(make_pair(a->getRelativePosition() + m->normal * 25.0f, Color(255, 255, 0)));
		//else
		//	a->debugPoints.push_back(make_pair(pointTransform * b->getRelativePosition() + m->normal * 25.0f, Color(255, 255, 0)));
#endif
	}
}

class PhysicsEngineGame : public Game
{
	vector<Body*> m_bodies;
	Body *m_selectedBody;
	Vector2F m_lastMousePosition;
	function<void(Manifold*)> m_manifoldGenerationFunctionTable[Shape::NUM_SHAPES][Shape::NUM_SHAPES];

	list<Manifold> m_manifolds;

	Vector2F m_gravity = Vector2F(0.0f, 150.0f);
	//Vector2F m_gravity = Vector2F(0.0f, 0.0f);

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


			{
				Vector2I size = getWindow()->getSize();

				Body *body = new Body();
				body->setPosition(Vector2F(size.x * 0.5f, size.y - 10.0f));
				body->setMass(0.0f);
				body->setInertia(0.0f);

				Box *ground = new Box;
				ground->setSize(Vector2F(size.x, 20.0f));
				body->addShape(ground);

				m_bodies.push_back(body);
			}
		}

		m_selectedBody = nullptr;
	}

	void cleanScene()
	{
		for(Body *body : m_bodies)
		{
			delete body;
		}
		m_bodies.clear();
		m_selectedBody = nullptr;
	}

	void onStart(GameEvent *e)
	{
		setupScene();
		
		getWindow()->getGraphicsContext()->setPointSize(5.0f);

		//m_manifoldGenerationFunctionTable[Shape::BOX][Shape::BOX] = manifolds::AABBToOBB;//manifolds::AABBToAABB;
		//m_manifoldGenerationFunctionTable[Shape::CIRCLE][Shape::BOX] = manifolds::CircleToAABB;
		//m_manifoldGenerationFunctionTable[Shape::BOX][Shape::CIRCLE] = manifolds::AABBToCircle;
		m_manifoldGenerationFunctionTable[Shape::CIRCLE][Shape::CIRCLE] = manifolds::CircleToCircle;
		m_manifoldGenerationFunctionTable[Shape::CIRCLE][Shape::POLYGON] = manifolds::CircleToPolygon;
		m_manifoldGenerationFunctionTable[Shape::POLYGON][Shape::CIRCLE] = manifolds::PolygonToCircle;
		m_manifoldGenerationFunctionTable[Shape::POLYGON][Shape::POLYGON] = manifolds::PolygonToPolygon;

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

		m_manifolds.clear();

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
						Manifold manifold(body, otherBody, shape, otherShape);
						m_manifoldGenerationFunctionTable[shape->getType()][otherShape->getType()](&manifold);
						if(manifold.contactPoints.size() > 0)
						{
							ResolveCollision(&manifold);
							CorrectPositions(&manifold);
							m_manifolds.push_back(manifold);
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
		for(Body *body : m_bodies)
		{
			Color color = body->m_isColliding ? Color::Blue : Color::White;
			
			// Render shape
			//Color color = Color::White;
			body->draw(e->getGraphicsContext(), color);

			// Draw velocity arrow
			e->getGraphicsContext()->drawArrow(body->getPosition(), body->getPosition() + body->getVelocity() * 0.1f, Color::Red);
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

	void ResolveCollision(Manifold *m)
	{
		float numContacts = m->contactPoints.size();
		for(Vector2F contactPoint : m->contactPoints)
		{
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
		switch(e->getKeycode())
		{
			case Keycode::SAUCE_KEY_1:
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

			case Keycode::SAUCE_KEY_2:
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

			case Keycode::SAUCE_KEY_3:
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

			case Keycode::SAUCE_KEY_4:
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

			case Keycode::SAUCE_KEY_5:
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