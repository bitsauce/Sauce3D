#pragma once

#include <Sauce/Sauce.h>

using namespace sauce;

struct MeshDesc : public SauceObjectDesc
{
	string meshFilePath = "";
};

class Mesh : public SauceObject<Mesh, MeshDesc>
{
public:
	Mesh()
		: m_vertexBuffer(nullptr)
	{
	}

	bool initialize(DescType) override;

	VertexBufferRef getVertexBuffer() const { return m_vertexBuffer; }

private:
	VertexBufferRef m_vertexBuffer;
};
SAUCE_TYPEDEFS(Mesh);
