#pragma once

#include <Sauce/Sauce.h>

#include "Shapes.h"

// TODO: Create a BodyDef class, maybe

class Body
{
public:
	Body() :
		m_position(0.0f, 0.0f),
		m_angle(0.0f),
		m_velocity(0.0f, 0.0f),
		m_angularVelocity(0.0f),
		m_staticFriction(0.2f),
		m_dynamicFriction(0.1f),
		m_restitution(0.5f)
	{
		setMass(0.001f);
		setInertia(1.0f);
	}

	void setPosition(Vector2F position)
	{
		m_position = position;
	}

	Vector2F getPosition() const
	{
		return m_position;
	}

	void setAngle(const float angle)
	{
		m_angle = angle;
	}

	float getAngle() const
	{
		return m_angle;
	}
	
	void move(Vector2F deltaPosition)
	{
		m_position += deltaPosition;
	}

	void rotate(const float angle)
	{
		m_angle += angle;
	}

	bool contains(const Vector2F point) const
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

	void draw(GraphicsContext *graphicsContext, Color color) const
	{
		graphicsContext->pushMatrix(bodyLocalToWorld());
		for(Shape *shape : m_shapes)
		{
			shape->draw(graphicsContext, color);
		}
		graphicsContext->popMatrix();
	}

	inline Matrix4 bodyLocalToWorld(Matrix4 *rotationOnlyMatrix = nullptr) const
	{
		// Body relative shape transforms to world transform
		Matrix4 localToWorld;
		localToWorld.rotateZ(math::radToDeg(m_angle));
		if(rotationOnlyMatrix) *rotationOnlyMatrix = localToWorld;
		localToWorld.translate(m_position.x, m_position.y, 0.0f);
		return localToWorld;
	}

	inline Matrix4 worldToBodyLocal(Matrix4 *rotationOnlyMatrix = nullptr) const
	{
		// World transform to body relative shape transforms
		Matrix4 worldToLocalRotation;
		worldToLocalRotation.rotateZ(-math::radToDeg(m_angle));
		if(rotationOnlyMatrix)
		{
			*rotationOnlyMatrix = worldToLocalRotation;
		}

		Matrix4 worldToLocal;
		worldToLocal.translate(-m_position.x, -m_position.y, 0.0f);
		worldToLocal = worldToLocalRotation * worldToLocal;
		return worldToLocal;
	}

	void addShape(Shape *shape)
	{
		m_shapes.push_back(shape);
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
	vector<Shape*> m_shapes;

	float m_mass, m_massInv;
	float m_inertia, m_inertiaInv;

	Vector2F m_position;
	float m_angle;

	Vector2F m_velocity;
	float m_angularVelocity;

	float m_staticFriction;
	float m_dynamicFriction;
	float m_restitution;
};
