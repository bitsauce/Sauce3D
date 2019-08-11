#pragma once

#include "Shapes.h"

class Manifold
{
public:
	Manifold(Body *bodyA, Body *bodyB, Shape *shapeA, Shape *shapeB) :
		bodyA(bodyA),
		bodyB(bodyB),
		shapeA(shapeA),
		shapeB(shapeB),
		penetration(0.0f),
		normal(0.0f)
	{
	}

	void swapAAndB()
	{
		Body *tmpBody = bodyA;
		const_cast<Body*>(bodyA) = bodyB;
		const_cast<Body*>(bodyB) = tmpBody;
		Shape *tmpShape = shapeA;
		const_cast<Shape*>(shapeA) = shapeB;
		const_cast<Shape*>(shapeB) = tmpShape;
	}

	Body * const bodyA;
	Body * const bodyB;
	Shape * const shapeA;
	Shape * const shapeB;
	float penetration;
	Vector2F normal;
	list<Vector2F> contactPoints;
};
