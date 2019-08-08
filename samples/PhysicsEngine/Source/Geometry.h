#pragma once

#include <Sauce/Sauce.h>
using namespace sauce;

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
		Vector2F normal;
		Vertex *v0, *v1;
		Edge *leftEdge, *rightEdge;
	};

	void init(const Vector2F *points, const Vector2F position, const Vector2F *normals, const int numPoints)
	{
		for(int i = 0; i < numPoints; i++)
		{
			Vertex *v = new Vertex(i);
			v->position = points[i] + position;
			v->localPosition = points[i];
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
			edge->normal = normals[i];
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

	vector<Vertex*> vertices;
	vector<Edge*> edges;
};
