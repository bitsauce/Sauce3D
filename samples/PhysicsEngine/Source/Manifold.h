#pragma once

#include "Body.h"
#include "Shapes.h"

// TODO: Manifold objects should maybe be connected to shapes
//       as then there will be less contactPoints.resize() calls

union ContactFeature
{
	struct
	{
		int8_t edge0;
		int8_t edge1;
	} edges;
	int16_t value;
};

struct Contact
{
	Vector2F position;
	float invMassNormal;
	float invMassTangent;

	float jAccum=0, jtAccum=0;

	ContactFeature feature;
};

class Manifold
{
public:
	static const int MAX_CONTACTS = 2;

	Manifold(Body *bodyA, Body *bodyB, Shape *shapeA, Shape *shapeB) :
		penetration(0.0f),
		normal(0.0f, 0.0f),
		tangent(0.0f, 0.0f),
		numContacts(0),
		unused(false)
	{
		if(false && shapeA < shapeB)
		{
			this->bodyA = bodyA;
			this->bodyB = bodyB;
			this->shapeA = shapeA;
			this->shapeB = shapeB;
		}
		else
		{
			// Swap a and b for coherence
			this->bodyB = bodyA;
			this->bodyA = bodyB;
			this->shapeB = shapeA;
			this->shapeA = shapeB;
		}
	}

	// TODO: Verify that this function works as expected when A > B
	void swapAAndB()
	{
		Body *tmpBody = bodyA;
		bodyA = bodyB;
		bodyB = tmpBody;
		Shape *tmpShape = shapeA;
		shapeA = shapeB;
		shapeB = tmpShape;
	}

	void update(Manifold &newManifold)
	{
		assert(bodyA == newManifold.bodyA && bodyB == newManifold.bodyB);
		assert(shapeA == newManifold.shapeA && shapeB == newManifold.shapeB);

		if(!g_accumulateImpulses)
		{
			Contact *newContacts = newManifold.contacts; int numNewContacts = newManifold.numContacts;

			for(int i = 0; i < numNewContacts; ++i)
			{
				contacts[i] = newContacts[i];
			}
			numContacts = numNewContacts;

			normal = newManifold.normal;
			penetration = newManifold.penetration;
			unused = false;
			return;
		}

		static Contact mergedContacts[MAX_CONTACTS];

		for(int i = 0; i < newManifold.numContacts; ++i)
		{
			// Find matching contact in new manifold
			const Contact &cNew = newManifold.contacts[i];
			int index = -1;
			for(int j = 0; j < numContacts; j++)
			{
				const Contact &cOld = contacts[j];
				if(cNew.feature.value == cOld.feature.value)
				{
					index = j;
					break;
				}
			}

			if(index >= 0)
			{
				// Matching contact found, setup accumulators
				Contact &c = mergedContacts[i];
				const Contact &cOld = contacts[index];
				c = cNew;
				c.jAccum = cOld.jAccum;
				//c.jtAccum = cOld.jtAccum;
			}
			else
			{
				// No matching contact found, copy new
				mergedContacts[i] = cNew;
			}
		}

		for(int i = 0; i < newManifold.numContacts; ++i)
		{
			contacts[i] = mergedContacts[i];
		}
		numContacts = newManifold.numContacts;

		normal = newManifold.normal;
		penetration = newManifold.penetration;
		unused = false;
	}

	Body *bodyA;
	Body *bodyB;
	Shape *shapeA;
	Shape *shapeB;
	float penetration;
	Vector2F normal;
	Vector2F tangent;

	Contact contacts[MAX_CONTACTS];
	int numContacts;

	bool unused;
};

struct ManifoldKey
{

	ManifoldKey(Shape *shapeA, Shape *shapeB)
	{
		if(false && shapeA < shapeB)
		{
			this->shapeA = shapeA;
			this->shapeB = shapeB;
		}
		else
		{
			// Swap a and b for coherence
			this->shapeB = shapeA;
			this->shapeA = shapeB;
		}
	}

	Shape *shapeA;
	Shape *shapeB;
};


inline bool operator < (const ManifoldKey& a1, const ManifoldKey& a2)
{
	if(a1.shapeA < a2.shapeA)
		return true;

	if(a1.shapeA == a2.shapeA && a1.shapeB < a2.shapeB)
		return true;

	return false;
}