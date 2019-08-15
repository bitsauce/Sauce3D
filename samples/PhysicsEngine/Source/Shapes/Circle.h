#pragma once

#include "Shape.h"

struct CircleDef : public ShapeDef
{
	CircleDef() :
		radius(0.0f)
	{
	}

	float radius;
};

class Circle : public Shape
{
public:
	Circle(const CircleDef &circleDef);

	void draw(GraphicsContext *graphicsContext, Color color) const override;
	bool contains(Vector2F point) const override;
	float getRadius() const { return m_radius; }

private:
	const float m_radius;
};
