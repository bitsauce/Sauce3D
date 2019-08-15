#include "Circle.h"

Circle::Circle(const CircleDef &circleDef) :
	Shape(circleDef, CIRCLE),
	m_radius(circleDef.radius)
{
	m_aabb.max = Vector2F(m_radius);
	m_aabb.min = -m_aabb.max;
	m_area = PI * m_radius * m_radius;
}

void Circle::draw(GraphicsContext *graphicsContext, Color color) const
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

	vertices[0].set2f(VERTEX_POSITION, 0.0f, 0.0f);
	vertices[0].set4ub(VERTEX_COLOR, color.getR(), color.getG(), color.getB(), color.getA());
	indices[0] = 0;
	indices[1] = 1;
	for(int i = 0; i < segments; ++i)
	{
		float r = (2.0f * PI * i) / segments;
		vertices[i+1].set2f(VERTEX_POSITION, cos(r) * m_radius, sin(r) * m_radius);
		vertices[i+1].set4ub(VERTEX_COLOR, color.getR(), color.getG(), color.getB(), color.getA());
		indices[(i+1)*2] = i+1;
		indices[(i+1)*2+1] = i+1 == segments ? 1 : i+2;
	}

	graphicsContext->pushMatrix(getBodyRelativeTransformation());
	graphicsContext->drawIndexedPrimitives(GraphicsContext::PRIMITIVE_LINES, vertices.data(), numVertices, indices.data(), numVertices * 2);
	graphicsContext->popMatrix();
}

bool Circle::contains(Vector2F point) const
{
	return (getBodyRelativePosition() - point).lengthSquared() < m_radius * m_radius;
}