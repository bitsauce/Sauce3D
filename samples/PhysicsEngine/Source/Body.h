#pragma once

#include <Sauce/Sauce.h>

#include "Shapes.h"
#include "PhysicsGrid.h"

struct BodyDef
{
	BodyDef() :
		mass(0.001f),
		inertia(1.0f),
		position(0.0f, 0.0f),
		angle(0.0f),
		velocity(0.0f, 0.0f),
		angularVelocity(0.0f),
		staticFriction(0.2f),
		dynamicFriction(0.1f),
		restitution(0.5f)
	{
	}

	float mass;
	float inertia;

	Vector2F position;
	float angle;
	
	Vector2F velocity;
	float angularVelocity;
	
	float staticFriction;
	float dynamicFriction;
	float restitution;

	list<Shape*> shapes;
};

class Body
{
public:
	Body(const BodyDef bodyDef, PhysicsGrid *physicsGrid) :
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

	void setPosition(Vector2F position)
	{
		const AABB aabbBefore = getAABB();
		m_position = m_positionPrev = position;
		m_transformsDirty = true;
		m_physicsGrid->bodyMoved(this, aabbBefore);
	}

	Vector2F getPosition() const
	{
		return m_position;
	}

	void setAngle(const float angle)
	{
		m_physicsGrid->removeBody(this);
		m_angle = m_anglePrev = angle;
		m_transformsDirty = true;
		m_physicsGrid->addBody(this);
	}

	float getAngle() const
	{
		return m_angle;
	}

	AABB getAABB()
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
	
	void move(const Vector2F &deltaPosition, const float angle)
	{
		const AABB aabbBefore = getAABB();
		m_position += deltaPosition;
		m_angle += angle;
		m_transformsDirty = true;
		m_physicsGrid->bodyMoved(this, aabbBefore);
	}

	bool contains(const Vector2F point)
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

	void draw(GraphicsContext *graphicsContext, Color color, float alpha);

	Matrix4 bodyLocalToWorld(Matrix4 *rotationOnlyMatrix = nullptr)
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

	inline Matrix4 bodyLocalToWorld(const float alpha, Matrix4 *rotationOnlyMatrix = nullptr) const
	{
		// Body relative shape transforms to world transform
		Matrix4 localToWorld;
		localToWorld.rotateZ(math::radToDeg(math::lerp(m_anglePrev, m_angle, alpha)));
		if(rotationOnlyMatrix) *rotationOnlyMatrix = localToWorld;
		const Vector2F position = math::lerp(m_positionPrev, m_position, alpha);
		localToWorld.translate(position.x, position.y, 0.0f);
		return localToWorld;
	}

	Matrix4 worldToBodyLocal(Matrix4 *rotationOnlyMatrix = nullptr)
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

	inline Matrix4 worldToBodyLocal(const float alpha, Matrix4 *rotationOnlyMatrix = nullptr) const
	{
		// World transform to body relative shape transforms
		Matrix4 worldToLocalRotation;
		worldToLocalRotation.rotateZ(-math::radToDeg(math::lerp(m_anglePrev, m_angle, alpha)));
		if(rotationOnlyMatrix)
		{
			*rotationOnlyMatrix = worldToLocalRotation;
		}

		Matrix4 worldToLocal;
		const Vector2F position = math::lerp(m_positionPrev, m_position, alpha);
		worldToLocal.translate(-position.x, -position.y, 0.0f);
		worldToLocal = worldToLocalRotation * worldToLocal;
		return worldToLocal;
	}

	vector<Shape*> &getShapes()
	{
		return m_shapes;
	}

	float getMass() const { return m_mass; }
	float getMassInv() const { return m_massInv; }
	void setMass(const float mass) { m_mass = mass; m_massInv = mass > 0.0f ? 1.0f / mass : 0.0f; }
	bool isStatic() const { return m_mass <= 0.0f; }

	float getInertia() const { return m_inertia; }
	float getInertiaInv() const { return m_inertiaInv; }
	void setInertia(const float inertia) { m_inertia = inertia; m_inertiaInv = inertia > 0.0f ? 1.0f / inertia : 0.0f; }

	float getRestitution() const { return m_restitution; }
	float setRestitution(const float restitution) { m_restitution = restitution; }

	Vector2F getVelocity() const { return m_velocity; }
	void setVelocity(const Vector2F velocity) { m_velocity = velocity; }

	float getAngularVelocity() const { return m_angularVelocity; }
	void setAngularVelocity(const float angularVelocity) { m_angularVelocity = angularVelocity; }

	float getStaticFriction() const { return m_staticFriction; }
	void setStaticFriction(const float staticFriction) { m_staticFriction = staticFriction; }

	float getDynamicFriction() const { return m_dynamicFriction; }
	void setDynamicFriction(const float dynamicFriction) { m_dynamicFriction = dynamicFriction; }

	void applyImpulse(const Vector2F &impulse, const Vector2F &radius)
	{
		m_velocity += impulse * m_massInv;
		m_angularVelocity += radius.cross(impulse) * m_inertiaInv;
	}

	bool m_isColliding = false;

private:
	void addShape(Shape *shape)
	{
		//m_physicsGrid->removeBody(this);
		// TODO: Careful! Have to update AABB when shape changes it local position
		AABB aabb = shape->getAABB(); // + shape->localPosition
		aabb.min += shape->getLocalPosition();
		aabb.max += shape->getLocalPosition();

		m_nonTransformedAABB.min.x = math::minimum(m_nonTransformedAABB.min.x, aabb.min.x);
		m_nonTransformedAABB.min.y = math::minimum(m_nonTransformedAABB.min.y, aabb.min.y);
		m_nonTransformedAABB.max.x = math::maximum(m_nonTransformedAABB.max.x, aabb.max.x);
		m_nonTransformedAABB.max.y = math::maximum(m_nonTransformedAABB.max.y, aabb.max.y);

		//m_physicsGrid->addBody(this);

		m_shapes.push_back(shape);
	}

	void updateBodyToWorldMatrices()
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

	PhysicsGrid *m_physicsGrid;

	AABB m_aabb;
	AABB m_nonTransformedAABB;

	vector<Shape*> m_shapes;

	float m_mass, m_massInv;
	float m_inertia, m_inertiaInv;

	Vector2F m_position, m_positionPrev;
	float m_angle, m_anglePrev;

	Vector2F m_velocity;
	float m_angularVelocity;

	float m_staticFriction;
	float m_dynamicFriction;
	float m_restitution;

	bool m_transformsDirty;
	Matrix4 m_bodyLocalToWorld;
	Matrix4 m_bodyLocalToWorldRotationsOnly;
	Matrix4 m_worldToBodyLocal;
	Matrix4 m_worldToBodyLocalRotationsOnly;
};
