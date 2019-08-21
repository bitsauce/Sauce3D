#include "Polygon.h"

PolygonShape::PolygonShape(const PolygonShapeDef &polygonShapeDef) :
	Shape(polygonShapeDef, POLYGON)
{
	initialize(&polygonShapeDef.vertices[0], polygonShapeDef.vertices.size());
}

PolygonShape::PolygonShape(const ShapeDef &shapeDef) :
	Shape(shapeDef, POLYGON)
{
}

void PolygonShape::initialize(const Vector2F *points, const int numPoints)
{
	// TODO: Add an automatic centering option
	// TODO: Assert convexness

	const Matrix4 bodyToShape = getBodyRelativeTransformation();

	free();

	m_aabb = AABB();
	m_aabb.max = m_aabb.min = bodyToShape * points[0];
	m_area = 0.0f;
	localCentroid = Vector2F();
	for(int i = 0; i < numPoints; i++)
	{
		Vertex *v = new Vertex(i);
		v->localPosition = v->position = bodyToShape * points[i];
		vertices.push_back(v);
		localCentroid += v->localPosition;

		m_aabb.min.x = math::minimum(m_aabb.min.x, v->localPosition.x);
		m_aabb.min.y = math::minimum(m_aabb.min.y, v->localPosition.y);
		m_aabb.max.x = math::maximum(m_aabb.max.x, v->localPosition.x);
		m_aabb.max.y = math::maximum(m_aabb.max.y, v->localPosition.y);

		m_area += points[i].cross(points[(i+1)%numPoints]);
	}
	m_area /= 2;
	localCentroid /= numPoints;
	centroid = localCentroid;

	//Let 'vertices' be an array of N pairs(x, y), indexed from 0
	//	Let 'area' = 0.0
	//	for i = 0 to N-1, do
	//		Let j = (i+1) mod N
	//		Let area = area + vertices[i].x * vertices[j].y
	//		Let area = area - vertices[i].y * vertices[j].x
	//		end for

	Edge *previousEdge = nullptr;
	for(int i = 0; i < numPoints; i++)
	{
		Edge *edge = new Edge(i);
		edge->v0 = vertices[i];
		edge->v0->rightEdge = edge;
		edge->v1 = vertices[(i + 1) % numPoints];
		edge->v1->leftEdge = edge;
		edge->localNormal = edge->normal = math::perp((edge->v0->localPosition - edge->v1->localPosition).normalized());
		if(previousEdge)
		{
			edge->leftEdge = previousEdge;
			previousEdge->rightEdge = edge;
		}
		previousEdge = edge;
		edges.push_back(edge);
	}
	edges[0]->leftEdge = previousEdge;
	previousEdge->rightEdge = edges[0];

	numVerticesAndEdges = vertices.size();
}

void PolygonShape::setTransform(const Matrix4 &pointTransform, const Matrix4 &normalTransform)
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

void PolygonShape::resetTransform()
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

void PolygonShape::free()
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

bool PolygonShape::contains(Vector2F point) const
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
