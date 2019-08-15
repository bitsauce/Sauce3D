#pragma once

#include "Shape.h"

// TODO:
// [ ] Add an automatic centering option
// [ ] Check convexness
// [ ] Improve encapsulation

struct PolygonShapeDef : public ShapeDef
{
	PolygonShapeDef()
	{
	}

	vector<Vector2F> vertices;
	//bool centerPolygon;
};

class PolygonShape : public Shape
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

	PolygonShape(const PolygonShapeDef &polygonShapeDef);

	void resetTransform();
	void setTransform(const Matrix4 &pointTransform, const Matrix4 &normalTransform);
	void free();
	void draw(GraphicsContext *graphicsContext, Color color) const;
	bool contains(Vector2F point) const;

	vector<Vertex*> vertices;
	vector<Edge*> edges;
	int numVerticesAndEdges;
	Vector2F localCentroid;
	Vector2F centroid;

protected:
	PolygonShape(const ShapeDef &shapeDef);
	void initialize(const Vector2F *vertices, const int numVertices);
};
