#pragma once

#include "../Config.h"
#include "../Utils.h"

struct ShapeDef
{
	ShapeDef() :
		bodyRelativePosition(0.0f, 0.0f),
		bodyRelativeAngle(0.0f),
		friction(1.0f),
		restitution(0.0f)
	{
	}

	Vector2F bodyRelativePosition;
	float bodyRelativeAngle;

	float friction;
	float restitution;
};

class Shape
{
public:
	enum Type
	{
		POLYGON,
		CIRCLE,
		NUM_SHAPES
	};

	Shape(const ShapeDef &shapeDef, const Type type) :
		m_type(type),
		m_bodyRelativePosition(shapeDef.bodyRelativePosition),
		m_bodyRelativeAngle(shapeDef.bodyRelativeAngle),
		m_friction(shapeDef.friction),
		m_restitution(shapeDef.restitution)
	{
	}

	Vector2F getBodyRelativePosition() const { return m_bodyRelativePosition; }
	float getBodyRelativeAngle() const { return m_bodyRelativeAngle; }
	float getRestitution() const { return m_restitution; }
	float getFriction() const { return m_friction; }

	Matrix4 getBodyRelativeTransformation() const
	{
		Matrix4 matrix;
		matrix.rotateZ(math::radToDeg(m_bodyRelativeAngle));
		matrix.translate(m_bodyRelativePosition.x, m_bodyRelativePosition.y, 0.0f);
		return matrix;
	}

	AABB getAABB() const
	{
		return m_aabb;
	}

	float getArea() const
	{
		return m_area;
	}

	virtual void draw(GraphicsContext *graphicsContext, Color color) const = 0;
	virtual bool contains(Vector2F point) const = 0;

	Type getType() const { return m_type; }

	list<pair<Vector2F, Color>> debugPoints;

protected:
	AABB m_aabb;
	float m_area;

private:
	const Type m_type;
	const Vector2F m_bodyRelativePosition;
	const float m_bodyRelativeAngle;
	const float m_friction;
	const float m_restitution;
};
