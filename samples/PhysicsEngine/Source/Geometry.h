#pragma once

#include <Sauce/Sauce.h>
using namespace sauce;

Vector2F perp(const Vector2F &v)
{
	return Vector2F(-v.y, v.x);
}

struct PhysicsPolygon
{
public:
	struct Edge;

	struct Vertex
	{
		Vertex(const int id) :
			id(id),
			leftEdge(nullptr),
			rightEdge(nullptr)
		{
		}

		const int id;
		Vector2F localPosition;
		Vector2F position;
		Edge *leftEdge, *rightEdge;
	};

	struct Edge
	{
		Edge(int id) :
			id(id),
			v0(nullptr),
			v1(nullptr),
			leftEdge(nullptr),
			rightEdge(nullptr)
		{
		}

		const int id;
		Vector2F localNormal;
		Vector2F normal;
		Vertex *v0, *v1;
		Edge *leftEdge, *rightEdge;
	};

	//PhysicsPolygon(const int numPoints)
	//{
	//	vertices.resize(numPoints);
	//}

	void init(const Vector2F *points, const int numPoints)
	{
		// TODO: Assert convexness
		if(vertices.size() > 0) free();

		for(int i = 0; i < numPoints; i++)
		{
			Vertex *v = new Vertex(i);
			v->localPosition = v->position = points[i];
			vertices.push_back(v);
		}

		Edge *previousEdge = nullptr;
		for(int i = 0; i < numPoints; i++)
		{
			Edge *edge = new Edge(i);
			edge->v0 = vertices[i];
			edge->v0->rightEdge = edge;
			edge->v1 = vertices[(i + 1) % numPoints];
			edge->v1->leftEdge = edge;
			edge->localNormal = edge->normal = perp((edge->v0->localPosition - edge->v1->localPosition).normalized()); // TODO: Cache all axies of the polygon
			if(previousEdge)
			{
				edge->leftEdge = previousEdge;
				previousEdge->rightEdge = edge;
			}
			previousEdge = edge;
			edges.push_back(edge);
		}
		edges[0]->leftEdge = edges[numPoints - 1];
	}

	void setTransform(const Matrix4 &pointTransform, const Matrix4 &normalTransform)
	{
		for(Vertex *v : vertices)
		{
			v->position = pointTransform * v->localPosition;
		}

		for(Edge *e : edges)
		{
			e->normal = normalTransform * e->localNormal;
		}
	}

	void free()
	{
		for(Vertex *v : vertices)
		{
			delete v;
		}
		vertices.clear();

		for(Edge *v : edges)
		{
			delete v;
		}
		edges.clear();
	}

	vector<Vertex*> vertices;
	vector<Edge*> edges;
};
