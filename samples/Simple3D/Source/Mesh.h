#pragma once

#include <Sauce/Sauce.h>

using namespace sauce;

class Mesh
{
public:
	Mesh(Vertex *vertices, uint vertexCount) :
		m_vertices(vertices), m_vertexCount(vertexCount)
	{}

	Vertex *getVertices() const { return m_vertices; }
	uint getVertexCount() const { return m_vertexCount;  }

private:
	Vertex *m_vertices;
	uint m_vertexCount;
};

Mesh *loadMesh(const string &modelFile);
