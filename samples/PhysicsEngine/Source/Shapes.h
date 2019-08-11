#pragma once

#include <Sauce/Sauce.h>
using namespace sauce;

#include "Geometry.h"

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

	virtual void draw(GraphicsContext *graphicsContext, Color color) const = 0;
	virtual bool contains(Vector2F point) const = 0;

	Type getType() const { return m_type; }

	list<pair<Vector2F, Color>> debugPoints;

private:
	const Type m_type;
	Vector2F m_localPosition;
	float m_localAngle;
};

class PolygonShape : public Shape
{
public:
	PolygonShape() :
		Shape(POLYGON)
	{
	}

	void initialize(const Vector2F *points, const int numPoints)
	{
		// TODO: Add an automatic centering option
		m_polygon.init(points, numPoints);
	}

	void draw(GraphicsContext *graphicsContext, Color color) const override
	{
		// Static draw data
		static vector<Vertex> vertices;
		static vector<uint> indices;

		// Generate vertex and index arrays
		const vector<PhysicsPolygon::Vertex*> &polygonVertices = m_polygon.vertices;
		const int numVertices = polygonVertices.size();
		for(int i = 0; i < numVertices; i++)
		{
			if(i >= vertices.size())
			{
				vertices.push_back(Vertex());
				indices.push_back(uint());
				indices.push_back(uint());
			}

			const Vector2F pos = polygonVertices[i]->localPosition;
			vertices[i].set2f(VERTEX_POSITION, pos.x, pos.y);
			vertices[i].set4ub(VERTEX_COLOR, color.getR(), color.getG(), color.getB(), color.getA());
			indices[i*2] = i;
			indices[i*2+1] = (i + 1) % numVertices;
		}

		// Draw outline
		graphicsContext->drawIndexedPrimitives(GraphicsContext::PRIMITIVE_LINES, vertices.data(), numVertices, indices.data(), numVertices * 2);

		// Draw box normals
		for(int i = 0; i < numVertices; i++)
		{
			const PhysicsPolygon::Edge *edge = m_polygon.edges[i];
			const Vector2F edgeNormal = edge->localNormal;
			const Vector2F edgeCenter = (edge->v0->localPosition + edge->v1->localPosition) * 0.5f;
			graphicsContext->drawArrow(edgeCenter, edgeCenter + edgeNormal * 15.0f, Color::Blue);
		}

		// Draw debug points
		for(pair<Vector2F, Color> p : debugPoints)
		{
			Vertex v;
			v.set2f(VERTEX_POSITION, p.first.x, p.first.y);
			v.set4ub(VERTEX_COLOR, p.second.getR(), p.second.getG(), p.second.getB(), p.second.getA());
			graphicsContext->drawPrimitives(GraphicsContext::PRIMITIVE_POINTS, &v, 1);
		}
	}

	bool contains(Vector2F point) const override
	{
		for(PhysicsPolygon::Edge *edge : m_polygon.edges)
		{
			if(edge->localNormal.dot(edge->v0->localPosition - point) < 0.0f)
			{
				return false;
			}
		}
		return true;
	}

	PhysicsPolygon *getPolygon(Matrix4 normalTransform = Matrix4(), Matrix4 pointTransform = Matrix4())
	{
		m_polygon.setTransform(pointTransform, normalTransform);
		return &m_polygon;
	}

private:
	PhysicsPolygon m_polygon;
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
		const int segments = 32;
		const int numVertices = segments + 1;

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
