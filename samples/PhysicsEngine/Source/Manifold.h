#pragma once

#include "Body.h"
#include "Shapes.h"

// TODO: Manifold objects should maybe be connected to shapes
//       as then there will be less contactPoints.resize() calls
class Manifold
{
public:
	Manifold() :
		bodyA(nullptr),
		bodyB(nullptr),
		shapeA(nullptr),
		shapeB(nullptr),
		penetration(0.0f),
		normal(0.0f),
		numContacts(0)
	{
		contactPoints.resize(10); // TODO: Common place for these constants
	}

	void initialize(Body *bodyA, Body *bodyB, Shape *shapeA, Shape *shapeB)
	{
		const_cast<Body*>(this->bodyA) = bodyA;
		const_cast<Body*>(this->bodyB) = bodyB;
		const_cast<Shape*>(this->shapeA) = shapeA;
		const_cast<Shape*>(this->shapeB) = shapeB;
		numContacts = 0;
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

	void addContactPoints(const Vector2F &contactPoint)
	{
		if(numContacts >= contactPoints.size())
		{
			contactPoints.resize(contactPoints.size() + 10);
		}
		contactPoints[numContacts++] = contactPoint;
	}

	Vector2F getContactPoint(int i) const
	{
		return contactPoints[i];
	}

	Body * const bodyA;
	Body * const bodyB;
	Shape * const shapeA;
	Shape * const shapeB;
	float penetration;
	Vector2F normal;
	int numContacts;

private:
	vector<Vector2F> contactPoints;
};
