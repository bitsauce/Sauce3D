#pragma once

#include <Sauce/Sauce.h>
using namespace sauce;

#include "Config.h"

void Body::draw(GraphicsContext *graphicsContext, Color color, float alpha)
{
	graphicsContext->pushMatrix(bodyLocalToWorld(alpha));
	for(Shape *shape : m_shapes)
	{
		shape->draw(graphicsContext, color);
	}
	graphicsContext->popMatrix();

#if DRAW_AABB == 1
	AABB aabb = getAABB();
	graphicsContext->drawRectangleOutline(RectF(aabb.min, aabb.max - aabb.min), Color::Yellow);
#endif // DRAW_AABB

	m_positionPrev = m_position;
	m_anglePrev = m_angle;
}

void PolygonShape::draw(GraphicsContext *graphicsContext, Color color) const
{
	// Static draw data
	static vector<sauce::Vertex> drawVertices;
	static vector<uint> indices;

	// Generate vertex and index arrays
	const vector<Vertex*> &polygonVertices = vertices;
	for(int i = 0; i < numVerticesAndEdges; i++)
	{
		if(i >= drawVertices.size())
		{
			drawVertices.push_back(sauce::Vertex());
			indices.push_back(uint());
			indices.push_back(uint());
		}

		const Vector2F pos = polygonVertices[i]->localPosition;
		drawVertices[i].set2f(VERTEX_POSITION, pos.x, pos.y);
		drawVertices[i].set4ub(VERTEX_COLOR, color.getR(), color.getG(), color.getB(), color.getA());
		indices[i*2] = i;
		indices[i*2+1] = (i + 1) % numVerticesAndEdges;
	}

	// Draw outline
	graphicsContext->drawIndexedPrimitives(GraphicsContext::PRIMITIVE_LINES, drawVertices.data(), numVerticesAndEdges, indices.data(), numVerticesAndEdges * 2);

#if DRAW_NORMALS == 1
	// Draw box normals
	for(int i = 0; i < numVerticesAndEdges; i++)
	{
		const Edge *edge = edges[i];
		const Vector2F edgeNormal = edge->localNormal;
		const Vector2F edgeCenter = (edge->v0->localPosition + edge->v1->localPosition) * 0.5f;
		graphicsContext->drawArrow(edgeCenter, edgeCenter + edgeNormal * 15.0f, Color::Blue);
	}
#endif // DRAW_NORMALS

	// Draw debug points
	for(pair<Vector2F, Color> p : debugPoints)
	{
		sauce::Vertex v;
		v.set2f(VERTEX_POSITION, p.first.x, p.first.y);
		v.set4ub(VERTEX_COLOR, p.second.getR(), p.second.getG(), p.second.getB(), p.second.getA());
		graphicsContext->drawPrimitives(GraphicsContext::PRIMITIVE_POINTS, &v, 1);
	}
}
