#pragma once

#include <Sauce/Sauce.h>

#include "Shapes.h"

// TODO: Create a BodyDef class, maybe

class Body
{
public:
	Body() :
		m_position(0.0f, 0.0f),
		m_positionPrev(0.0f, 0.0f),
		m_angle(0.0f),
		m_anglePrev(0.0f),
		m_velocity(0.0f, 0.0f),
		m_angularVelocity(0.0f),
		m_staticFriction(0.2f),
		m_dynamicFriction(0.1f),
		m_restitution(0.5f),
		m_transformsDirty(true)
	{
		setMass(0.001f);
		setInertia(1.0f);
	}

	void setPosition(Vector2F position)
	{
		m_position = m_positionPrev = position;
		m_transformsDirty = true;
	}

	Vector2F getPosition() const
	{
		return m_position;
	}

	void setAngle(const float angle)
	{
		m_angle = m_anglePrev = angle;
		m_transformsDirty = true;
	}

	float getAngle() const
	{
		return m_angle;
	}
	
	void move(Vector2F deltaPosition)
	{
		m_position += deltaPosition;
		m_transformsDirty = true;
	}

	void rotate(const float angle)
	{
		m_angle += angle;
		m_transformsDirty = true;
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

	void draw(GraphicsContext *graphicsContext, Color color, float alpha)
	{
		graphicsContext->pushMatrix(bodyLocalToWorld(alpha));
		for(Shape *shape : m_shapes)
		{
			shape->draw(graphicsContext, color);
		}
		graphicsContext->popMatrix();
		m_positionPrev = m_position;
		m_anglePrev = m_angle;
	}

	Matrix4 bodyLocalToWorld(Matrix4 *rotationOnlyMatrix = nullptr)
	{
		//return bodyLocalToWorld(0,rotationOnlyMatrix); //problem
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
		//return worldToBodyLocal(0, rotationOnlyMatrix);
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

		m_transformsDirty = false;
	}

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
