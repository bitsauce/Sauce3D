#pragma once

#include "Polygon.h"

struct BoxDef : public ShapeDef
{
	BoxDef() :
		size(0.0f, 0.0f),
		angle(0.0f)
	{
	}

	Vector2F size;
	float angle;
};

class Box : public PolygonShape
{
public:
	Box(const BoxDef &boxDef);

	Vector2F getSize() const { return m_size; }
	float getAngle() const { return m_angle; }

private:
	const Vector2F m_size;
	const float m_angle;
};
