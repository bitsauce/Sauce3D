#include "PhysicsWorld.h"
#include "Constants.h"
#include "Body.h"
#include "Shapes.h"
#include "Manifold.h"
#include "Collision.h"
#include "Scenes.h"

using namespace sauce;

PhysicsWorld::PhysicsWorld() :
	m_timeScale(1.0f)
{
}

void PhysicsWorld::initialize()
{
	// Initialize physics grid division
	Window *window = Game::Get()->getWindow();
	const Vector2I size = Vector2FInPhysicsSpace(window->getSize());
	const Vector2I numCells = (size / g_physicsCellSize) + Vector2I(1);
	m_physicsGrid.initialize(numCells);

	// Set point size for debug visualization
	window->getGraphicsContext()->setPointSize(5.0f);

	// Setup manifold generation function table
	m_manifoldGenerationFunctionTable[Shape::CIRCLE][Shape::CIRCLE] = collision::CircleToCircle;
	m_manifoldGenerationFunctionTable[Shape::CIRCLE][Shape::POLYGON] = collision::CircleToPolygon;
	m_manifoldGenerationFunctionTable[Shape::POLYGON][Shape::CIRCLE] = collision::PolygonToCircle;
	m_manifoldGenerationFunctionTable[Shape::POLYGON][Shape::POLYGON] = collision::PolygonToPolygon;
}

void PhysicsWorld::broadphase()
{
	// Broadphase - We use a grid division of the physics world to retrieve nearby bodies
	const Vector2I physicsGridNumCells = m_physicsGrid.getNumCells();
	for(int y = 0; y < physicsGridNumCells.y; ++y)
	{
		for(int x = 0; x < physicsGridNumCells.x; ++x)
		{
			list<Body*> bodies = m_physicsGrid.getBodiesInCell(Vector2I(x, y));
			for(auto itrI = bodies.begin(); itrI != bodies.end();)
			{
				Body *body = *itrI;
				itrI++;
				for(auto itrJ = itrI; itrJ != bodies.end(); ++itrJ)
				{
					Body *otherBody = *itrJ;

					// Skip collision between static objects
					if(body->isStatic() && otherBody->isStatic())
						continue;

					// Check that bodies' AABB overlap
					if(!body->getAABB().overlaps(otherBody->getAABB()))
						continue;

					// Check collision on every shape of each body
					for(Shape *shape : body->getShapes())
					{
						for(Shape *otherShape : otherBody->getShapes())
						{
							ManifoldKey manifoldKey(shape, otherShape);
							Manifold manifold(body, otherBody, shape, otherShape);
							m_manifoldGenerationFunctionTable[manifold.shapeA->getType()][manifold.shapeB->getType()](&manifold);
							if(manifold.numContacts > 0)
							{
								ManifoldItr itr = m_manifolds.find(manifoldKey);
								if(itr == m_manifolds.end())
								{
									m_manifolds.insert(make_pair(manifoldKey, manifold));
								}
								else
								{
									itr->second.update(manifold);
								}

								body->m_isColliding = otherBody->m_isColliding = true;
							}
							else
							{
								m_manifolds.erase(manifoldKey);
							}
						}
					}
				}
			}
		}
	}
}

void PhysicsWorld::update(float dt)
{
	const float timeScale = 1.0f;
	dt = dt * m_timeScale;

	// Determine overlapping bodies and update contact points
	broadphase();

	// Integrate forces
	for(Body *body : m_bodies)
	{
		// Reset some per-tick variables
		body->m_isColliding = false;
		for(Shape *shape : body->getShapes())
			shape->debugPoints.clear();

		if(body->isStatic())
			continue;

		body->setVelocity(body->getVelocity() + g_defaultGravity * dt);

		//b->velocity += dt * (gravity + b->invMass * b->force);
		//b->angularVelocity += dt * b->invI * b->torque;
	}

	list<ManifoldKey> manifoldsToRemove;
	for(ManifoldItr itr = m_manifolds.begin(); itr != m_manifolds.end(); ++itr)
	{
		// Remove manifolds that went untouched
		if(itr->second.unused)
		{
			manifoldsToRemove.push_back(itr->first);
			continue;
		}
		itr->second.unused = true;
		preStep(dt, &itr->second);
	}

	// Remove unused manifolds
	for(ManifoldKey key : manifoldsToRemove)
	{
		m_manifolds.erase(key);
	}

#if DISABLE_COLLISIONS != 1
	// Perform iterations
	for(int i = 0; i < g_iterations; ++i)
	{
		for(ManifoldItr itr = m_manifolds.begin(); itr != m_manifolds.end(); ++itr)
		{
			// Resolve collisions
			resolveCollision(&itr->second);
			//correctPositions(&itr->second);
		}
	}
#endif // DISABLE_COLLISIONS

	// Integrate velocities
	for(Body *body : m_bodies)
	{
		body->move(body->getVelocity() * dt, body->getAngularVelocity() * dt);
	}
}

void PhysicsWorld::preStep(const float dt, Manifold *m)
{
	const float k_allowedPenetration = 0.01f;
	float k_biasFactor = g_positionCorrection ? 0.2f : 0.0f;

	Body *a = m->bodyA;
	Body *b = m->bodyB;

	for(int i = 0; i < m->numContacts; ++i)
	{
		Contact &c = m->contacts[i];
		const Vector2F rAP = c.position - a->getPosition();
		const Vector2F rBP = c.position - b->getPosition();

		// Precompute inverse mass for normal impulse
		float rAPcrossN = rAP.cross(m->normal);
		float rBPcrossN = rBP.cross(m->normal);
		c.invMassNormal = a->getMassInv() + b->getMassInv() + (rAPcrossN * rAPcrossN * a->getInertiaInv()) + (rBPcrossN * rBPcrossN * b->getInertiaInv());

		// Precompute inverse mass for tangental impulse
		m->tangent = math::perp(m->normal);
		float rAPcrossT = rAP.cross(m->tangent);
		float rBPcrossT = rBP.cross(m->tangent);
		c.invMassTangent = a->getMassInv() + b->getMassInv() + (rAPcrossT * rAPcrossT * a->getInertiaInv()) + (rBPcrossT * rBPcrossT * b->getInertiaInv());

		c.bias = -k_biasFactor * (1.0f / dt) * math::minimum(0.0f, k_allowedPenetration - m->penetration);

		// Calculate relative velocity
		Vector2F relativeVelocity =
			b->getVelocity() + math::perp(rBP) * b->getAngularVelocity() -
			a->getVelocity() - math::perp(rAP) * a->getAngularVelocity();

		// Calculate relative velocity in terms of the normal direction
		float velocityAlongNormal = relativeVelocity.dot(m->normal);

		float e = min(m->shapeA->getRestitution(), m->shapeB->getRestitution());
		if(velocityAlongNormal < -1.0f)
		{
			c.bias = -e * velocityAlongNormal;
		}

		// Apply accumulated impulses
		if(g_accumulateImpulses)
		{
			// Apply normal + friction impulse
			const Vector2F impulse = m->normal * c.jAccum + m->tangent * c.jtAccum;
			a->applyImpulse(-impulse, rAP);
			b->applyImpulse(impulse, rBP);
		}
	}
}

void PhysicsWorld::resolveCollision(Manifold *m)
{
	Body *a = m->bodyA;
	Body *b = m->bodyB;

	for(int i = 0; i < m->numContacts; ++i)
	{
		Contact &c = m->contacts[i];

		Vector2F rAP = c.position - a->getPosition();
		Vector2F rBP = c.position - b->getPosition();

		// Calculate relative velocity
		Vector2F relativeVelocity =
			b->getVelocity() + math::perp(rBP) * b->getAngularVelocity() -
			a->getVelocity() - math::perp(rAP) * a->getAngularVelocity();

		// Calculate relative velocity in terms of the normal direction
		float velocityAlongNormal = relativeVelocity.dot(m->normal);

		// Calculate magnitude of impulse
		float j = -velocityAlongNormal + c.bias;
		j /= c.invMassNormal;

		if(g_accumulateImpulses)
		{
			// Clamp the accumulated impulse
			float jA = c.jAccum;
			c.jAccum = math::maximum(jA + j, 0.0f);
			j = c.jAccum - jA;
		}
		else
		{
			j = math::maximum(j, 0.0f);
		}

		// Apply normal impulse
		Vector2F impulse = m->normal * j;
		a->applyImpulse(-impulse, rAP);
		b->applyImpulse(impulse, rBP);
			
		// Recompute relative velocity
		relativeVelocity =
			b->getVelocity() + math::perp(rBP) * b->getAngularVelocity() -
			a->getVelocity() - math::perp(rAP) * a->getAngularVelocity();

		// Solve for magnitude to apply along the friction vector
		float jt = -relativeVelocity.dot(m->tangent);
		jt /= c.invMassTangent;
						
		// Approximate friction coefficient
		float friction = std::sqrt(m->shapeA->getFriction() * m->shapeB->getFriction());

		// Clamp magnitude of friction and calculate impulse vector
		if(g_accumulateImpulses)
		{
			float maxJt = friction * c.jAccum;
			float jtA = c.jtAccum;
			c.jtAccum = math::clamp(jtA + jt, -maxJt, maxJt);
			jt = c.jtAccum - jtA;
		}
		else
		{
			float maxJt = friction * j;
			jt = math::clamp(jt, -maxJt, maxJt);
		}

		// Apply friction
		const Vector2F frictionImpulse = m->tangent * jt;
		a->applyImpulse(-frictionImpulse, rAP);
		b->applyImpulse(frictionImpulse, rBP);
	}
}

void PhysicsWorld::correctPositions(Manifold *m)
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
