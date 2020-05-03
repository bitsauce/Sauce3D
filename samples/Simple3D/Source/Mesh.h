#pragma once

#include <Sauce/Sauce.h>

using namespace sauce;

struct MeshDesc : public SauceObjectDesc
{
	string meshFilePath = "";
};

class Mesh : public SauceObject
{
public:
	SAUCE_REF_TYPE(Mesh);

	Mesh()
		: m_vertexBuffer(nullptr)
	{
	}

	bool initialize(MeshDesc meshDesc);

	VertexBufferRef getVertexBuffer() const { return m_vertexBuffer; }

private:
	VertexBufferRef m_vertexBuffer;
};
SAUCE_REF_TYPE_TYPEDEFS(Mesh);
