// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#pragma once

#include <Sauce/Common.h>

BEGIN_SAUCE_NAMESPACE

/*********************************************************************
**	Vertex attributes												**
**********************************************************************/
enum class VertexAttribute : uint32
{
	Position,
	Color,
	TexCoord,
	Normal,
	Max
};

/*********************************************************************
**	Vertex format													**
**********************************************************************/
class Vertex;
class SAUCE_API VertexFormat
{
	friend class Game;
	friend class Vertex;
	friend class VertexBuffer;
public:
	VertexFormat();
	VertexFormat(const VertexFormat &other);

	void set(const VertexAttribute attrib, const int size, const Datatype=Datatype::Float);
	int getElementCount(const VertexAttribute attrib) const;
	Datatype getDatatype(const VertexAttribute attrib) const;
	bool isAttributeEnabled(const VertexAttribute attrib) const;

	uint getVertexSizeInBytes() const;
	uint getAttributeOffset(const VertexAttribute attrib) const;
	
	Vertex *createVertices(const uint32 count) const;
	
	VertexFormat &operator=(const VertexFormat &other);
	bool operator==(const VertexFormat &other);

protected:
	static VertexFormat s_vct; // Position, color, texture coord

private:
	struct Attribute
	{
		Attribute() :
			elementCount(0),
			datatype(Datatype::Float),
			offset(0)
		{
		}

		int elementCount;
		Datatype datatype;
		uint offset;
		
		bool operator!=(const Attribute &other)
		{
			return elementCount != other.elementCount || datatype != other.datatype;
		}
	};

	Attribute m_attributes[(uint32)VertexAttribute::Max];
	uint m_vertexByteSize;
};

/*********************************************************************
**	Vertex															**
**********************************************************************/
class SAUCE_API Vertex
{
public:
	Vertex();
	Vertex(const Vertex &other);
	Vertex(const VertexFormat &fmt);
	~Vertex();

	void setFormat(const VertexFormat &fmt);
	VertexFormat getFormat() const;

	void set1f(const VertexAttribute attrib, const float v0);
	void set1ui(const VertexAttribute attrib, const uint v0);
	void set1i(const VertexAttribute attrib, const int v0);
	void set1us(const VertexAttribute attrib, const ushort v0);
	void set1s(const VertexAttribute attrib, const short v0);
	void set1ub(const VertexAttribute attrib, const uchar v0);
	void set1b(const VertexAttribute attrib, const char v0);

	void set2f(const VertexAttribute attrib, const float v0, const float v1);
	void set2ui(const VertexAttribute attrib, const uint v0, const uint v1);
	void set2i(const VertexAttribute attrib, const int v0, const int v1);
	void set2us(const VertexAttribute attrib, const ushort v0, const ushort v1);
	void set2s(const VertexAttribute attrib, const short v0, const short v1);
	void set2ub(const VertexAttribute attrib, const uchar v0, const uchar v1);
	void set2b(const VertexAttribute attrib, const char v0, const char v1);

	void set3f(const VertexAttribute attrib, const float v0, const float v1, const float v2);
	void set3ui(const VertexAttribute attrib, const uint v0, const uint v1, const uint v2);
	void set3i(const VertexAttribute attrib, const int v0, const int v1, const int v2);
	void set3us(const VertexAttribute attrib, const ushort v0, const ushort v1, const ushort v2);
	void set3s(const VertexAttribute attrib, const short v0, const short v1, const short v2);
	void set3ub(const VertexAttribute attrib, const uchar v0, const uchar v1, const uchar v2);
	void set3b(const VertexAttribute attrib, const char v0, const char v1, const char v2);

	void set4f(const VertexAttribute attrib, const float v0, const float v1, const float v2, const float v3);
	void set4ui(const VertexAttribute attrib, const uint v0, const uint v1, const uint v2, const uint v3);
	void set4i(const VertexAttribute attrib, const int v0, const int v1, const int v2, const int v3);
	void set4us(const VertexAttribute attrib, const ushort v0, const ushort v1, const ushort v2, const ushort v3);
	void set4s(const VertexAttribute attrib, const short v0, const short v1, const short v2, const short v3);
	void set4ub(const VertexAttribute attrib, const uchar v0, const uchar v1, const uchar v2, const uchar v3);
	void set4b(const VertexAttribute attrib, const char v0, const char v1, const char v2, const char v3);

	void setData(const char *data) const;
	void getData(char *data) const;
	
	Vertex &operator=(const Vertex &other);

	void print();

private:
	char *m_data;
	VertexFormat m_format;
};

END_SAUCE_NAMESPACE
