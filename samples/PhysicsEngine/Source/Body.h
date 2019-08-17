#pragma once

#include "Config.h"
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
		friction(1.0f),
		restitution(0.5f)
	{
	}

	float mass;
	float inertia;

	Vector2F position;
	float angle;
	
	Vector2F velocity;
	float angularVelocity;
	
	float friction;
	float restitution;

	list<Shape*> shapes;
};

class Body
{
public:
	Body(const BodyDef bodyDef, PhysicsGrid *physicsGrid);
	void setPosition(Vector2F position);
	Vector2F getPosition() const;
	void setAngle(const float angle);
	float getAngle() const;
	AABB getAABB();
	void move(const Vector2F &deltaPosition, const float angle);
	void applyImpulse(const Vector2F &impulse, const Vector2F &radius);
	bool contains(const Vector2F point);
	void draw(GraphicsContext *graphicsContext, Color color, float alpha);
	Matrix4 bodyLocalToWorld(Matrix4 *rotationOnlyMatrix = nullptr);
	Matrix4 worldToBodyLocal(Matrix4 *rotationOnlyMatrix = nullptr);

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

	float getFriction() const { return m_friction; }
	void setFriction(const float friction) { m_friction = friction; }

	list<Shape*> &getShapes() { return m_shapes; }

	bool m_isColliding = false;

private:
	void addShape(Shape *shape);
	void updateBodyToWorldMatrices();

	PhysicsGrid *m_physicsGrid;

	AABB m_aabb;
	AABB m_nonTransformedAABB;

	list<Shape*> m_shapes;

	float m_mass, m_massInv;
	float m_inertia, m_inertiaInv;

	Vector2F m_position, m_positionPrev;
	float m_angle, m_anglePrev;

	Vector2F m_velocity;
	float m_angularVelocity;

	float m_friction;
	float m_restitution;

	bool m_transformsDirty;
	Matrix4 m_bodyLocalToWorld;
	Matrix4 m_bodyLocalToWorldRotationsOnly;
	Matrix4 m_worldToBodyLocal;
	Matrix4 m_worldToBodyLocalRotationsOnly;
};
