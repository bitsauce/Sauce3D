#include "Body.h"
#include "Constants.h"

Body::Body(const BodyDef bodyDef, PhysicsGrid *physicsGrid) :
	m_position(bodyDef.position),
	m_positionPrev(bodyDef.position),
	m_angle(bodyDef.angle),
	m_anglePrev(bodyDef.angle),
	m_velocity(bodyDef.velocity),
	m_angularVelocity(bodyDef.angularVelocity),
	m_staticFriction(bodyDef.staticFriction),
	m_dynamicFriction(bodyDef.dynamicFriction),
	m_restitution(bodyDef.restitution),
	m_transformsDirty(true),
	m_physicsGrid(physicsGrid)
{
	assert(bodyDef.shapes.size() > 0);

	// Add shapes (will set the AABB)
	//float totalArea = 0.0f;
	for(Shape *shape : bodyDef.shapes)
	{
		addShape(shape);
		//totalArea += shape->getArea();
	}

	// Add body to physics grid
	physicsGrid->addBody(this);

	// Set mass and inertia
	setMass(bodyDef.mass);
	setInertia(bodyDef.inertia);
}

void Body::setPosition(Vector2F position)
{
	const AABB aabbBefore = getAABB();
	m_position = m_positionPrev = position;
	m_transformsDirty = true;
	m_physicsGrid->bodyMoved(this, aabbBefore);
}

Vector2F Body::getPosition() const
{
	return m_position;
}

void Body::setAngle(const float angle)
{
	m_physicsGrid->removeBody(this);
	m_angle = m_anglePrev = angle;
	m_transformsDirty = true;
	m_physicsGrid->addBody(this);
}

float Body::getAngle() const
{
	return m_angle;
}

AABB Body::getAABB()
{
	AABB aabb = m_nonTransformedAABB;
		
	Matrix4 bodyToWorld = bodyLocalToWorld();
	static Vector2F corners[4];
	corners[0] = bodyToWorld * Vector2F(aabb.min.x, aabb.min.y);
	corners[1] = bodyToWorld * Vector2F(aabb.max.x, aabb.min.y);
	corners[2] = bodyToWorld * Vector2F(aabb.max.x, aabb.max.y);
	corners[3] = bodyToWorld * Vector2F(aabb.min.x, aabb.max.y);

	aabb.max = aabb.min = corners[0];
	for(int i = 1; i < 4; ++i)
	{
		aabb.min.x = math::minimum(aabb.min.x, corners[i].x);
		aabb.min.y = math::minimum(aabb.min.y, corners[i].y);
		aabb.max.x = math::maximum(aabb.max.x, corners[i].x);
		aabb.max.y = math::maximum(aabb.max.y, corners[i].y);
	}

	return aabb;
}
	
void Body::move(const Vector2F &deltaPosition, const float angle)
{
	const AABB aabbBefore = getAABB();
	m_position += deltaPosition;
	m_angle += angle;
	m_transformsDirty = true;
	m_physicsGrid->bodyMoved(this, aabbBefore);
}

bool Body::contains(const Vector2F point)
{
	const Vector2F transformedPoint = worldToBodyLocal() * point;
	for(Shape *shape : m_shapes)
	{
		if(shape->contains(transformedPoint))
		{
			return true;
		}
	}
	return false;
}

void Body::draw(GraphicsContext *graphicsContext, Color color, float alpha)
{
	const Vector2F position = math::lerp(m_positionPrev, m_position, alpha);
	const float angle = math::lerp(m_anglePrev, m_angle, alpha);

	// Body relative shape transforms to world transform
	Matrix4 localToWorld;
	localToWorld.rotateZ(math::radToDeg(angle));
	localToWorld.translate(position.x, position.y, 0.0f);

	graphicsContext->pushMatrix(localToWorld);
	for(Shape *shape : m_shapes)
	{
		shape->draw(graphicsContext, color);
	}
	graphicsContext->popMatrix();

#if DRAW_AABB == 1
	AABB aabb = getAABB();
	graphicsContext->drawRectangleOutline(RectF(aabb.min, aabb.max - aabb.min), Color::Yellow);
#endif // DRAW_AABB

	m_positionPrev = m_position;
	m_anglePrev = m_angle;
}

Matrix4 Body::bodyLocalToWorld(Matrix4 *rotationOnlyMatrix)
{
	if(m_transformsDirty)
	{
		updateBodyToWorldMatrices();
	}

	if(rotationOnlyMatrix)
	{
		*rotationOnlyMatrix = m_bodyLocalToWorldRotationsOnly;
	}

	return m_bodyLocalToWorld;
}

Matrix4 Body::worldToBodyLocal(Matrix4 *rotationOnlyMatrix)
{
	if(m_transformsDirty)
	{
		updateBodyToWorldMatrices();
	}

	if(rotationOnlyMatrix)
	{
		*rotationOnlyMatrix = m_worldToBodyLocalRotationsOnly;
	}

	return m_worldToBodyLocal;
}

void Body::applyImpulse(const Vector2F &impulse, const Vector2F &radius)
{
	m_velocity += impulse * m_massInv;
	m_angularVelocity += radius.cross(impulse) * m_inertiaInv;
}

void Body::addShape(Shape *shape)
{
	//m_physicsGrid->removeBody(this);
	// TODO: Careful! Have to update AABB when shape changes it local position
	AABB aabb = shape->getAABB(); // + shape->localPosition
	aabb.min += shape->getBodyRelativePosition();
	aabb.max += shape->getBodyRelativePosition();

	m_nonTransformedAABB.min.x = math::minimum(m_nonTransformedAABB.min.x, aabb.min.x);
	m_nonTransformedAABB.min.y = math::minimum(m_nonTransformedAABB.min.y, aabb.min.y);
	m_nonTransformedAABB.max.x = math::maximum(m_nonTransformedAABB.max.x, aabb.max.x);
	m_nonTransformedAABB.max.y = math::maximum(m_nonTransformedAABB.max.y, aabb.max.y);

	//m_physicsGrid->addBody(this);

	m_shapes.push_back(shape);
}

void Body::updateBodyToWorldMatrices()
{
	// Body relative shape transforms to world transform
	m_bodyLocalToWorld = Matrix4();
	m_bodyLocalToWorld.rotateZ(math::radToDeg(m_angle));
	m_bodyLocalToWorldRotationsOnly = m_bodyLocalToWorld; // Copy rotation only matrix here
	m_bodyLocalToWorld.translate(m_position.x, m_position.y, 0.0f);

	// World transform to body relative shape transforms
	m_worldToBodyLocalRotationsOnly = Matrix4();
	m_worldToBodyLocalRotationsOnly.rotateZ(-math::radToDeg(m_angle));
	m_worldToBodyLocal = Matrix4();
	m_worldToBodyLocal.translate(-m_position.x, -m_position.y, 0.0f);
	m_worldToBodyLocal = m_worldToBodyLocalRotationsOnly * m_worldToBodyLocal;

	// Update AABB
	/*m_aabb.min = Vector2F(FLT_MAX);
	m_aabb.max = Vector2F(-FLT_MAX);
	for(Shape *shape : m_shapes)
	{
		switch(shape->getType())
		{
			case Shape::POLYGON:
			{
				PolygonShape *polygon = dynamic_cast<PolygonShape*>(shape);
				for(int i = 0; i < polygon->numVerticesAndEdges; ++i)
				{

					m_aabb.min.x = math::minimum(m_aabb.min.x, v->localPosition.x);
					m_aabb.min.y = math::minimum(m_aabb.min.y, v->localPosition.y);
					m_aabb.max.x = math::maximum(m_aabb.max.x, v->localPosition.x);
					m_aabb.max.y = math::maximum(m_aabb.max.y, v->localPosition.y);

					polygon->vertices[i]->localPosition;
				}
			}
			break;
		}
	}*/

	m_transformsDirty = false;
}
