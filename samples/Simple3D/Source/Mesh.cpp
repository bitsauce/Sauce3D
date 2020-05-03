#include "Mesh.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

bool Mesh::initialize(MeshDesc meshDesc)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, meshDesc.meshFilePath.c_str());

	if (!err.empty())
	{
		// "err" may contain warning messages
		LOG(err.c_str());
	}

	if (!ret)
	{
		return false;
	}

	VertexFormat fmt;
	fmt.set(VertexAttribute::VERTEX_POSITION, 3, Datatype::Float);
	fmt.set(VertexAttribute::VERTEX_NORMAL, 3, Datatype::Float);
	fmt.set(VertexAttribute::VERTEX_TEX_COORD, 2, Datatype::Float);

	// Loop over shapes
	for (size_t s = 0; s < shapes.size(); ++s)
	{
		// Create vertices
		uint32 vertexCount = shapes[s].mesh.indices.size();
		Vertex* vertices = fmt.createVertices(vertexCount);

		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
		{
			int fv = shapes[s].mesh.num_face_vertices[f];

			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++)
			{
				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
				tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
				tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
				tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
				tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
				tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];
				tinyobj::real_t tx = attrib.texcoords[2 * idx.texcoord_index + 0];
				tinyobj::real_t ty = attrib.texcoords[2 * idx.texcoord_index + 1];
				// Optional: vertex colors
				// tinyobj::real_t red = attrib.colors[3*idx.vertex_index+0];
				// tinyobj::real_t green = attrib.colors[3*idx.vertex_index+1];
				// tinyobj::real_t blue = attrib.colors[3*idx.vertex_index+2];
				vertices[index_offset + v].set3f(VertexAttribute::VERTEX_POSITION, vx, vy, vz);
				vertices[index_offset + v].set3f(VertexAttribute::VERTEX_NORMAL, nx, ny, nz);
				vertices[index_offset + v].set2f(VertexAttribute::VERTEX_TEX_COORD, tx, ty);
			}
			index_offset += fv;

			// per-face material
			shapes[s].mesh.material_ids[f];
		}

		VertexBufferDesc vertexBufferDesc;
		vertexBufferDesc.vertices = vertices;
		vertexBufferDesc.vertexCount = vertexCount;
		m_vertexBuffer = CreateNew<VertexBuffer>(vertexBufferDesc);

		delete[] vertices;

		return true;
	}
	return true;
}
