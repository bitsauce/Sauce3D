#pragma once

#include <Sauce/Sauce.h>
using namespace sauce;

Vector2F perp(const Vector2F &v);

struct AABB
{
	Vector2F min, max;

	bool overlaps(const AABB &other) const
	{
		return min.x <= other.max.x &&
			max.x >= other.min.x &&
			min.y <= other.max.y &&
			max.y >= other.min.y;
	}

	bool contains(const Vector2F &point) const
	{
		return point.x >= min.x &&
			point.x <= max.x &&
			point.y <= max.y &&
			point.y >= min.y;
	}

	AABB intersection(const AABB &other) const
	{
		AABB aabb;
		aabb.min.x = math::maximum(min.x, other.min.x);
		aabb.min.y = math::maximum(min.y, other.min.y);
		aabb.max.x = math::minimum(max.x, other.max.x);
		aabb.max.y = math::minimum(max.y, other.max.y);
		return aabb;
	}
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

	Shape(Type type) :
		m_type(type),
		m_localPosition(0.0f, 0.0f),
		m_localAngle(0.0f)
	{
	}

	void setLocalPosition(Vector2F relativePosition)
	{
		m_localPosition = relativePosition;
	}

	Vector2F getLocalPosition() const
	{
		return m_localPosition;
	}

	AABB getAABB() const
	{
		return m_aabb;
	}

	virtual void draw(GraphicsContext *graphicsContext, Color color) const = 0;
	virtual bool contains(Vector2F point) const = 0;

	Type getType() const { return m_type; }

	list<pair<Vector2F, Color>> debugPoints;

protected:
	AABB m_aabb;

private:
	const Type m_type;
	Vector2F m_localPosition;
	float m_localAngle;
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

	PolygonShape() :
		Shape(POLYGON),
		numVerticesAndEdges(0)
	{
	}

	void initialize(const Vector2F *points, const int numPoints)
	{
		// TODO: Add an automatic centering option
		// TODO: Assert convexness

		free();

		m_aabb = AABB();
		m_aabb.max = m_aabb.min = points[0];
		localCentroid = Vector2F();
		for(int i = 0; i < numPoints; i++)
		{
			Vertex *v = new Vertex(i);
			v->localPosition = v->position = points[i];
			vertices.push_back(v);
			localCentroid += v->localPosition;

			m_aabb.min.x = math::minimum(m_aabb.min.x, v->localPosition.x);
			m_aabb.min.y = math::minimum(m_aabb.min.y, v->localPosition.y);
			m_aabb.max.x = math::maximum(m_aabb.max.x, v->localPosition.x);
			m_aabb.max.y = math::maximum(m_aabb.max.y, v->localPosition.y);
		}
		localCentroid /= numPoints;
		centroid = localCentroid;

		Edge *previousEdge = nullptr;
		for(int i = 0; i < numPoints; i++)
		{
			Edge *edge = new Edge(i);
			edge->v0 = vertices[i];
			edge->v0->rightEdge = edge;
			edge->v1 = vertices[(i + 1) % numPoints];
			edge->v1->leftEdge = edge;
			edge->localNormal = edge->normal = perp((edge->v0->localPosition - edge->v1->localPosition).normalized());
			if(previousEdge)
			{
				edge->leftEdge = previousEdge;
				previousEdge->rightEdge = edge;
			}
			previousEdge = edge;
			edges.push_back(edge);
		}
		edges[0]->leftEdge = edges[numPoints - 1];

		numVerticesAndEdges = vertices.size();
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

		centroid = pointTransform * localCentroid;
	}

	void resetTransform()
	{
		for(Vertex *v : vertices)
		{
			v->position = v->localPosition;
		}

		for(Edge *e : edges)
		{
			e->normal = e->localNormal;
		}

		centroid = localCentroid;
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

		numVerticesAndEdges = 0;
	}

	void draw(GraphicsContext *graphicsContext, Color color) const override;

	bool contains(Vector2F point) const override
	{
		for(Edge *edge : edges)
		{
			if(edge->localNormal.dot(edge->v0->localPosition - point) < 0.0f)
			{
				return false;
			}
		}
		return true;
	}

	vector<Vertex*> vertices;
	vector<Edge*> edges;
	int numVerticesAndEdges;
	Vector2F localCentroid;
	Vector2F centroid;
	AABB aabb;
};

class Box : public PolygonShape
{
public:
	Box()
	{
		setSize(Vector2F(10.f, 10.f));
	}

	Vector2F getSize() const
	{
		return m_size;
	}

	void setSize(const Vector2F size)
	{
		// TODO: Add dirty flag
		m_size = size;
		const Vector2F position = getLocalPosition();
		const Vector2F halfSize = size * 0.5f;
		const Vector2F corners[4] = {
			 Vector2F(position.x - halfSize.x, position.y - halfSize.y),
			 Vector2F(position.x + halfSize.x, position.y - halfSize.y),
			 Vector2F(position.x + halfSize.x, position.y + halfSize.y),
			 Vector2F(position.x - halfSize.x, position.y + halfSize.y)
		};
		initialize(corners, 4);
	}

private:
	Vector2F m_size;
};

class Circle : public Shape
{
public:
	Circle() :
		Shape(CIRCLE),
		m_radius(10.0f)
	{
	}

	void draw(GraphicsContext *graphicsContext, Color color) const override
	{
		// Static draw data
		static vector<Vertex> vertices;
		static vector<uint> indices;
		static const int segments = 32;
		static const int numVertices = segments + 1;

		// Make sure we have enough vertices
		if(vertices.size() < numVertices)
		{
			vertices.resize(numVertices);
			indices.resize(numVertices * 2);
		}

		const Vector2F position = getLocalPosition();
		vertices[0].set2f(VERTEX_POSITION, position.x, position.y);
		vertices[0].set4ub(VERTEX_COLOR, color.getR(), color.getG(), color.getB(), color.getA());
		indices[0] = 0;
		indices[1] = 1;
		for(int i = 0; i < segments; ++i)
		{
			float r = (2.0f * PI * i) / segments;
			vertices[i+1].set2f(VERTEX_POSITION, position.x + cos(r) * m_radius, position.y + sin(r) * m_radius);
			vertices[i+1].set4ub(VERTEX_COLOR, color.getR(), color.getG(), color.getB(), color.getA());
			indices[(i+1)*2] = i+1;
			indices[(i+1)*2+1] = i+1 == segments ? 1 : i+2;
		}

		graphicsContext->drawIndexedPrimitives(GraphicsContext::PRIMITIVE_LINES, vertices.data(), numVertices, indices.data(), numVertices * 2);
	}

	bool contains(Vector2F point) const override
	{
		return (getLocalPosition() - point).lengthSquared() < m_radius * m_radius;
	}

	float getRadius() const { return m_radius; }
	void setRadius(const float rad) { m_radius = rad; }

private:
	float m_radius;
};
