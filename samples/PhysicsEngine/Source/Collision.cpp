#include <Sauce/Sauce.h>
#include "Config.h"
#include "Body.h"
#include "Shapes.h"
#include "Manifold.h"

using namespace sauce;

PolygonShape::Vertex *GetFarthestPoint(PolygonShape *polygon, Vector2F dir)
{
	float maxDist = -FLT_MAX;
	int index = 0;
	for(int i = 0; i < polygon->vertices.size(); i++)
	{
		float dist = polygon->vertices[i]->position.dot(dir);
		if(dist > maxDist)
		{
			maxDist = dist;
			index = i;
		}
	}
	return polygon->vertices[index];
}

vector<Vector2F> ClipEdge(Vector2F v0, Vector2F v1, Vector2F n, float o)
{
	vector<Vector2F> clipEdges;
	clipEdges.reserve(2);

	float d0 = n.dot(v0) - o;
	float d1 = n.dot(v1) - o;

	if(d0 >= 0.0f) clipEdges.push_back(v0);
	if(d1 >= 0.0f) clipEdges.push_back(v1);

	if(d0 * d1 < 0.0f)
	{
		float u = d0 / (d0 - d1);
		Vector2F e = (v1 - v0) * u + v0;
		clipEdges.push_back(e);
	}

	return clipEdges;
}

Vector2F perp(const Vector2F &v)
{
	return Vector2F(-v.y, v.x);
}

namespace collision
{
	void CircleToCircle(Manifold *m)
	{
		Circle *circleA = dynamic_cast<Circle*>(m->shapeA);
		Circle *circleB = dynamic_cast<Circle*>(m->shapeB);
		Body *bodyA = m->bodyA;
		Body *bodyB = m->bodyB;

		// Rotation only matrices (used later)
		Matrix4 worldToBodyARotationOnly;
		Matrix4 bodyAToWorldRotationOnly;
		Matrix4 bodyBToWorldRotationOnly;

		// Get body to world matrices
		Matrix4 bodyAToWorld = bodyA->bodyLocalToWorld(&bodyAToWorldRotationOnly);
		Matrix4 bodyBToWorld = bodyB->bodyLocalToWorld(&bodyBToWorldRotationOnly);

		// Get matrices for transforming from bodyB space to bodyA space
		Matrix4 bodyBToBodyA = bodyA->worldToBodyLocal(&worldToBodyARotationOnly) * bodyBToWorld;

		// Calculate vector from shape in bodyA to shape in bodyB
		const Vector2F normal = bodyBToBodyA * circleB->getLocalPosition() - circleA->getLocalPosition();
		const float lengthSquared = normal.lengthSquared();
		const float totalRadius = circleA->getRadius() + circleB->getRadius();

		// If their combined radius is less than the distance between, there is no contact
		if(lengthSquared >= totalRadius * totalRadius)
		{
			return;
		}

		// There is contact, calculate the distance using sqrt
		float distance = std::sqrt(lengthSquared);
		if(distance == 0.0f)
		{
			m->penetration = circleA->getRadius();
			m->addContactPoints(m->bodyA->getPosition());

			// If the circles are at the exact same point,
			// we'll simply use the following normal vector
			m->normal = Vector2F(1, 0);
;		}
		else
		{
			// Calculate the penetation and normal vector
			m->penetration = totalRadius - distance;

			// Normalize the vector from a to b (will be the collision normal)
			m->normal = normal / distance;

			// Calculate contact point
			m->addContactPoints(bodyAToWorld * (m->normal * circleA->getRadius() + circleA->getLocalPosition()));
		}

		// Transform collision normal to world space
		m->normal = bodyAToWorldRotationOnly * m->normal;
	}

	void PolygonToCircle(Manifold *m)
	{
		PolygonShape *a = dynamic_cast<PolygonShape*>(m->shapeA);
		Circle *b = dynamic_cast<Circle*>(m->shapeB);

		// Rotation only matrices (used later)
		Matrix4 worldToBodyARotationOnly;
		Matrix4 bodyAToWorldRotationOnly;
		Matrix4 bodyBToWorldRotationOnly;

		// Get body to world matrices
		Matrix4 bodyAToWorld = m->bodyA->bodyLocalToWorld(&bodyAToWorldRotationOnly);
		Matrix4 bodyBToWorld = m->bodyB->bodyLocalToWorld(&bodyBToWorldRotationOnly);

		// Get matrices for transforming from bodyB space to bodyA space
		Matrix4 bodyBToBodyA = m->bodyA->worldToBodyLocal(&worldToBodyARotationOnly) * bodyBToWorld;
		Matrix4 bodyBToBodyARotationOnly = worldToBodyARotationOnly * bodyBToWorldRotationOnly;

		// Calculate vector from shape in bodyA to shape in bodyB
		const Vector2F shapePositionB = bodyBToBodyA * b->getLocalPosition();
		const float radiusOfB = b->getRadius();
		const Vector2F deltaPosition = shapePositionB - a->getLocalPosition();

		a->setTransform(Matrix4(), Matrix4());
		float separation = -FLT_MAX;
		int faceNormal = 0;
		for(int i = 0; i < a->vertices.size(); i++)
		{
			float s = a->edges[i]->normal.dot(shapePositionB - a->vertices[i]->position);
			if(s > radiusOfB)
			{
				return;
			}

			if(s > separation)
			{
				separation = s;
				faceNormal = i;
			}
		}

		PolygonShape::Edge *edge = a->edges[faceNormal];

		if(separation < 0.0f)
		{
			m->normal = edge->normal;
			m->addContactPoints(bodyAToWorld * (m->normal * radiusOfB + bodyBToBodyA * b->getLocalPosition()));
			m->normal = bodyAToWorldRotationOnly * m->normal;
			m->penetration = radiusOfB;
			return;
		}

		float dot1 = (shapePositionB - edge->v0->position).dot(edge->v1->position - edge->v0->position);
		float dot2 = (shapePositionB - edge->v1->position).dot(edge->v0->position - edge->v1->position);
		if(dot1 <= 0.0f)
		{
			if((shapePositionB - edge->v0->position).lengthSquared() > radiusOfB * radiusOfB)
			{
				return;
			}

			m->normal = -(edge->v0->position - shapePositionB).normalized();
			m->addContactPoints(bodyAToWorld * (edge->v0->position + a->getLocalPosition()));
		}
		else if(dot2 <= 0.0f)
		{
			if((shapePositionB - edge->v1->position).lengthSquared() > radiusOfB * radiusOfB)
			{
				return;
			}

			m->normal = -(edge->v1->position - shapePositionB).normalized();
			m->addContactPoints(bodyAToWorld * (edge->v1->position + a->getLocalPosition()));
		}
		else
		{
			if(edge->normal.dot(shapePositionB - edge->v0->position) > radiusOfB)
			{
				return;
			}

			m->normal = edge->normal;
			m->addContactPoints(bodyAToWorld * (-m->normal * radiusOfB + bodyBToBodyA * b->getLocalPosition()));
		}
		m->normal = bodyAToWorldRotationOnly * m->normal;
	}

	void CircleToPolygon(Manifold *m)
	{
		m->swapAAndB();
		PolygonToCircle(m);
	}

	void PolygonToPolygon(Manifold *m)
	{
		PolygonShape *polygonA = dynamic_cast<PolygonShape*>(m->shapeA);
		PolygonShape *polygonB = dynamic_cast<PolygonShape*>(m->shapeB);
		Body *bodyA = m->bodyA;
		Body *bodyB = m->bodyB;

		// Rotation only matrices (used later)
		Matrix4 worldToBodyARotationOnly;
		Matrix4 bodyAToWorldRotationOnly;
		Matrix4 bodyBToWorldRotationOnly;

		// Get body to world matrices
		Matrix4 bodyAToWorld = bodyA->bodyLocalToWorld(&bodyAToWorldRotationOnly);
		Matrix4 bodyBToWorld = bodyB->bodyLocalToWorld(&bodyBToWorldRotationOnly);

		// Get matrices for transforming from bodyB space to bodyA space
		Matrix4 bodyBToBodyA = bodyA->worldToBodyLocal(&worldToBodyARotationOnly) * bodyBToWorld;
		Matrix4 bodyBToBodyARotationOnly = worldToBodyARotationOnly * bodyBToWorldRotationOnly;

		polygonA->resetTransform(); // TODO: This could be completely removed with some work
		polygonB->setTransform(bodyBToBodyA, bodyBToBodyARotationOnly);

		// Calculate vector from shape in bodyA to shape in bodyB
		const Vector2F deltaPositions = polygonB->centroid - polygonA->centroid;

		static vector<Vector2F> axes;
		int axisMaxSize = axes.size();
		int numAxis = 0;

		for(PolygonShape::Edge *edge : polygonA->edges)
		{
			// An optimization is made here:
			// We assume that normals pointing in the opposite direction of
			// the delta positions are highly unlikely to be the axis of least
			// peneration, so we pre-cull them here
			if(edge->normal.dot(deltaPositions) > 0.0f)
			{
				if(numAxis >= axisMaxSize)
				{
					axes.resize(axisMaxSize + 100);
					axisMaxSize += 100;
				}
				axes[numAxis++] = edge->normal;
			}
		}

		for(PolygonShape::Edge *edge : polygonB->edges)
		{
			if(edge->normal.dot(deltaPositions) > 0.0f)
			{
				if(numAxis >= axisMaxSize)
				{
					axes.resize(axisMaxSize + 100);
					axisMaxSize += 100;
				}
				axes[numAxis++] = edge->normal;
			}
		}

		// Determine if the shapes are overlapping by iterating every axis of both shapes
		// and checking the separation along every axis (SAT)
		//
		// Separating Axis Teorem (SAT):
		// In order to confirm a collision, overlapping on all axes has to be true -- 
		// if there's any axis without an overlap, we can conclude that there's no collision.
		//
		// For oriented boxes, there are 4 axes (normals) to check in total, 2 per box
		float leastPenetration = FLT_MAX; int axisOfLeastPenetration = -1;
		PolygonShape::Vertex *farthestCornerOfA = nullptr, *farthestCornerOfB = nullptr;
		int farthestCornerOfAIndex = -1, farthestCornerOfBIndex = -1;
		for(int i = 0; i < numAxis; i++)
		{
			const Vector2F axis = axes[i];

			// Find min and max extents when projected onto the axis
			float dotMinA = FLT_MAX;
			float dotMaxA = -FLT_MAX;
			{
				for(int j = 0; j < polygonA->numVerticesAndEdges; j++)
				{
					float dot = axis.dot(polygonA->vertices[j]->position);
					if(dot > dotMaxA)
					{
						dotMaxA = dot;
						farthestCornerOfAIndex = j;
					}
					if(dot < dotMinA)
					{
						dotMinA = dot;
					}
				}
			}

			float dotMinB = FLT_MAX;
			float dotMaxB = -FLT_MAX;
			{
				for(int j = 0; j < polygonB->numVerticesAndEdges; j++)
				{
					float dot = axis.dot(polygonB->vertices[j]->position);
					if(dot > dotMaxB)
					{
						dotMaxB = dot;
					}
					if(dot < dotMinB)
					{
						dotMinB = dot;
						farthestCornerOfBIndex = j;
					}
				}
			}

			float penetration;
			if(dotMaxA > dotMaxB)
			{
				penetration = dotMaxB - dotMinA;
			}
			else
			{
				penetration = dotMaxA - dotMinB;
			}

			// SAT: Return if no overlap along this axis
			if(penetration < 0.0f)
				return;

			if(penetration < leastPenetration)
			{
				leastPenetration = penetration;
				axisOfLeastPenetration = i;

				// Find the vertex farthest along +n for object a
				farthestCornerOfA = polygonA->vertices[farthestCornerOfAIndex];

				// Find the vertex farthest along -n for object b
				farthestCornerOfB = polygonB->vertices[farthestCornerOfBIndex];
			}
		}

		// Store axis of least penetation
		m->normal = axes[axisOfLeastPenetration];
		m->penetration = leastPenetration;

		// Determine best edge of a and b by considering the edges
		// neighbouring the fartest vertices, and picking the ones
		// that are most perpendicular to the collision normal

		// Select edge that is most perpendicular to separation normal
		// out of the two neighbouring edges of the farthest vertex of A
		PolygonShape::Edge *bestEdgeOfA =
			abs(farthestCornerOfA->leftEdge->normal.dot(m->normal)) > abs(farthestCornerOfA->rightEdge->normal.dot(m->normal)) ?
			farthestCornerOfA->leftEdge : farthestCornerOfA->rightEdge;

		// Repeat for B
		PolygonShape::Edge *bestEdgeOfB =
			abs(farthestCornerOfB->leftEdge->normal.dot(m->normal)) > abs(farthestCornerOfB->rightEdge->normal.dot(m->normal)) ?
			farthestCornerOfB->leftEdge : farthestCornerOfB->rightEdge;

		// Determine reference and incident edge
		// The reference edge is the edge most
		// perpendicular to the separation normal
		PolygonShape::Edge *referenceEdge, *incidentEdge;
		if(abs(bestEdgeOfA->normal.dot(m->normal)) < abs(bestEdgeOfB->normal.dot(m->normal)))
		{
			referenceEdge = bestEdgeOfA;
			incidentEdge = bestEdgeOfB;
		}
		else
		{
			referenceEdge = bestEdgeOfB;
			incidentEdge = bestEdgeOfA;
		}

		// Perform edge clipping
		Vector2F referenceEdgeVector = Vector2F(-referenceEdge->normal.y, referenceEdge->normal.x);

		float o = referenceEdgeVector.dot(referenceEdge->v0->position);
		vector<Vector2F> clippedEdge = ClipEdge(incidentEdge->v0->position, incidentEdge->v1->position, referenceEdgeVector, o);
		if(clippedEdge.size() < 2)
			return;

		o = referenceEdgeVector.dot(referenceEdge->v1->position);
		clippedEdge = ClipEdge(clippedEdge[0], clippedEdge[1], -referenceEdgeVector, -o);
		if(clippedEdge.size() < 2)
			return;

		Vector2F referenceEdgeNormal = referenceEdge->normal;
		if(referenceEdgeNormal.dot(clippedEdge[0] - referenceEdge->v0->position) < 0.0f)
		{
			m->addContactPoints(bodyAToWorld * clippedEdge[0]);
		}

		if(referenceEdgeNormal.dot(clippedEdge[1] - referenceEdge->v0->position) < 0.0f)
		{
			m->addContactPoints(bodyAToWorld * clippedEdge[1]);
		}

		// Transform collision normal to world space
		m->normal = bodyAToWorldRotationOnly * m->normal;

#ifdef DEBUG_DRAW
		Vector2F edge = referenceEdge->v1->position - referenceEdge->v0->position;
		for(float f = 0.0f; f < 1.0f; f += 0.1f)
		{
			a->debugPoints.push_back(make_pair(referenceEdge->v0->position + edge * f, Color(0, 255, 0)));
		}

		edge = incidentEdge->v1->position - incidentEdge->v0->position;
		for(float f = 0.0f; f < 1.0f; f += 0.1f)
		{
			a->debugPoints.push_back(make_pair(incidentEdge->v0->position + edge * f, Color(255, 0, 0)));
		}

		//a->debugPoints.push_back(make_pair(farthestCornerOfB->position, Color(0, 255, 0)));
		//a->debugPoints.push_back(make_pair(farthestCornerOfA->position, Color(0, 255, 0)));
		//if(minPenetrationAxis < 2)
		//	a->debugPoints.push_back(make_pair(a->getRelativePosition() + m->normal * 25.0f, Color(255, 255, 0)));
		//else
		//	a->debugPoints.push_back(make_pair(pointTransform * b->getRelativePosition() + m->normal * 25.0f, Color(255, 255, 0)));
#endif
	}
}
