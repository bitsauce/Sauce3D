//     _____                        ______             _            
//    / ____|                      |  ____|           (_)           
//   | (___   __ _ _   _  ___ ___  | |__   _ __   __ _ _ _ __   ___ 
//    \___ \ / _` | | | |/ __/ _ \ |  __| | '_ \ / _` | | '_ \ / _ \
//    ____) | (_| | |_| | (_|  __/ | |____| | | | (_| | | | | |  __/
//   |_____/ \__,_|\__,_|\___\___| |______|_| |_|\__, |_|_| |_|\___|
//                                                __/ |             
//                                               |___/              
// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#include <Sauce/Common.h>
#include <Sauce/Graphics.h>

BEGIN_SAUCE_NAMESPACE

/*********************************************************************
**	Vertex format													**
**********************************************************************/

VertexFormat VertexFormat::s_vtc;

VertexFormat::VertexFormat()
	: m_vertexByteSize(0)
{
}

VertexFormat::VertexFormat(const VertexFormat &other)
{
	for(uint32 i = 0; i < (uint32)VertexAttribute::Max; i++)
	{
		m_attributes[i] = other.m_attributes[i];
	}
	m_vertexByteSize = other.m_vertexByteSize;
}

void VertexFormat::set(const VertexAttribute attrib, const int size, const Datatype datatype)
{
	if(size >= 0 && size <= 4)
	{
		m_attributes[(uint32)attrib].elementCount = size;
		m_attributes[(uint32)attrib].datatype = datatype;

		m_vertexByteSize = 0;
		for(int i = 0; i < (uint32)VertexAttribute::Max; i++)
		{
			VertexAttribute at = VertexAttribute(i);
			if(isAttributeEnabled(at))
			{
				m_attributes[(uint32)at].offset = m_vertexByteSize;
				switch(getDatatype(at))
				{
				case Datatype::Float:
					m_vertexByteSize += sizeof(float)*getElementCount(at); break;
				case Datatype::Uint32:
				case Datatype::Int32:
					m_vertexByteSize += sizeof(int)*getElementCount(at); break;
				case Datatype::Uint16:
				case Datatype::Int16:
					m_vertexByteSize += sizeof(short)*getElementCount(at); break;
				case Datatype::Uint8:
				case Datatype::Int8:
					m_vertexByteSize += sizeof(char)*getElementCount(at); break;
				}
			}
		}
	}
	else
	{
		LOG("VertexFormat::set(): Size must be in the range [0, 4].");
	}
}

int VertexFormat::getElementCount(const VertexAttribute attrib) const
{
	return m_attributes[(uint32)attrib].elementCount;
}

Datatype VertexFormat::getDatatype(const VertexAttribute attrib) const
{
	return m_attributes[(uint32)attrib].datatype;
}

uint VertexFormat::getVertexSizeInBytes() const
{
	return m_vertexByteSize;
}

uint VertexFormat::getAttributeOffset(const VertexAttribute attrib) const
{
	return m_attributes[(uint32)attrib].offset;
}

bool VertexFormat::isAttributeEnabled(const VertexAttribute attrib) const
{
	return m_attributes[(uint32)attrib].elementCount != 0;
}

VertexArray VertexFormat::createVertices(const uint32 count) const
{
	return move(VertexArray(count, *this));
}

VertexFormat &VertexFormat::operator=(const VertexFormat &other)
{
	for(int i = 0; i < (uint32)VertexAttribute::Max; i++)
	{
		m_attributes[i] = other.m_attributes[i];
	}
	m_vertexByteSize = other.m_vertexByteSize;
	return *this;
}

bool VertexFormat::operator==(const VertexFormat &other)
{
	for(int i = 0; i < (uint32)VertexAttribute::Max; i++)
	{
		if(m_attributes[i] != other.m_attributes[i])
			return false;
	}
	return true;
}

/*********************************************************************
**	Vertex															**
**********************************************************************/

Vertex::Vertex()
	: m_vertexData(nullptr)
	, m_vertexFormat()
{
}

void Vertex::set1f(const VertexAttribute attrib, const float v0)
{
	if(m_vertexFormat.isAttributeEnabled(attrib) || m_vertexFormat.getElementCount(attrib) != 1)
	{
		((float*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[0] = v0;
	}
	else
	{
		LOG("Vertex format does not match");
	}
}

void Vertex::set1ui(const VertexAttribute attrib, const uint v0)
{
	if(m_vertexFormat.isAttributeEnabled(attrib) || m_vertexFormat.getElementCount(attrib) != 1)
	{
		((uint*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[0] = v0;
	}
	else
	{
		LOG("Vertex format does not match");
	}
}

void Vertex::set1i(const VertexAttribute attrib, const int v0)
{
	if(m_vertexFormat.isAttributeEnabled(attrib) || m_vertexFormat.getElementCount(attrib) != 1)
	{
		((int*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[0] = v0;
	}
	else
	{
		LOG("Vertex format does not match");
	}
}

void Vertex::set1us(const VertexAttribute attrib, const ushort v0)
{
	if(m_vertexFormat.isAttributeEnabled(attrib) || m_vertexFormat.getElementCount(attrib) != 1)
	{
		((ushort*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[0] = v0;
	}
	else
	{
		LOG("Vertex format does not match");
	}
}

void Vertex::set1s(const VertexAttribute attrib, const short v0)
{
	if(m_vertexFormat.isAttributeEnabled(attrib) || m_vertexFormat.getElementCount(attrib) != 1)
	{
		((short*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[0] = v0;
	}
	else
	{
		LOG("Vertex format does not match");
	}
}

void Vertex::set1ub(const VertexAttribute attrib, const uchar v0)
{
	if(m_vertexFormat.isAttributeEnabled(attrib) || m_vertexFormat.getElementCount(attrib) != 1)
	{
		((uchar*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[0] = v0;
	}
	else
	{
		LOG("Vertex format does not match");
	}
}

void Vertex::set1b(const VertexAttribute attrib, const char v0)
{
	if(m_vertexFormat.isAttributeEnabled(attrib) || m_vertexFormat.getElementCount(attrib) != 1)
	{
		((char*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[0] = v0;
	}
	else
	{
		LOG("Vertex format does not match");
	}
}

void Vertex::set2f(const VertexAttribute attrib, const float v0, const float v1)
{
	if(m_vertexFormat.isAttributeEnabled(attrib) || m_vertexFormat.getElementCount(attrib) != 2)
	{
		((float*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[0] = v0;
		((float*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[1] = v1;
	}
	else
	{
		LOG("Vertex format does not match");
	}
}

void Vertex::set2ui(const VertexAttribute attrib, const uint v0, const uint v1)
{
	if(m_vertexFormat.isAttributeEnabled(attrib) || m_vertexFormat.getElementCount(attrib) != 2)
	{
		((uint*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[0] = v0;
		((uint*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[1] = v1;
	}
	else
	{
		LOG("Vertex format does not match");
	}
}

void Vertex::set2i(const VertexAttribute attrib, const int v0, const int v1)
{
	if(m_vertexFormat.isAttributeEnabled(attrib) || m_vertexFormat.getElementCount(attrib) != 2)
	{
		((int*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[0] = v0;
		((int*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[1] = v1;
	}
	else
	{
		LOG("Vertex format does not match");
	}
}

void Vertex::set2us(const VertexAttribute attrib, const ushort v0, const ushort v1)
{
	if(m_vertexFormat.isAttributeEnabled(attrib) || m_vertexFormat.getElementCount(attrib) != 2)
	{
		((ushort*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[0] = v0;
		((ushort*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[1] = v1;
	}
	else
	{
		LOG("Vertex format does not match");
	}
}

void Vertex::set2s(const VertexAttribute attrib, const short v0, const short v1)
{
	if(m_vertexFormat.isAttributeEnabled(attrib) || m_vertexFormat.getElementCount(attrib) != 2)
	{
		((short*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[0] = v0;
		((short*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[1] = v1;
	}
	else
	{
		LOG("Vertex format does not match");
	}
}

void Vertex::set2ub(const VertexAttribute attrib, const uchar v0, const uchar v1)
{
	if(m_vertexFormat.isAttributeEnabled(attrib) || m_vertexFormat.getElementCount(attrib) != 2)
	{
		((uchar*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[0] = v0;
		((uchar*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[1] = v1;
	}
	else
	{
		LOG("Vertex format does not match");
	}
}

void Vertex::set2b(const VertexAttribute attrib, const char v0, const char v1)
{
	if(m_vertexFormat.isAttributeEnabled(attrib) || m_vertexFormat.getElementCount(attrib) != 2)
	{
		((char*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[0] = v0;
		((char*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[1] = v1;
	}
	else
	{
		LOG("Vertex format does not match");
	}
}

void Vertex::set3f(const VertexAttribute attrib, const float v0, const float v1, const float v2)
{
	if(m_vertexFormat.isAttributeEnabled(attrib) || m_vertexFormat.getElementCount(attrib) != 3)
	{
		((float*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[0] = v0;
		((float*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[1] = v1;
		((float*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[2] = v2;
	}
	else
	{
		LOG("Vertex format does not match");
	}
}

void Vertex::set3ui(const VertexAttribute attrib, const uint v0, const uint v1, const uint v2)
{
	if(m_vertexFormat.isAttributeEnabled(attrib) || m_vertexFormat.getElementCount(attrib) != 3)
	{
		((uint*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[0] = v0;
		((uint*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[1] = v1;
		((uint*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[2] = v2;
	}
	else
	{
		LOG("Vertex format does not match");
	}
}

void Vertex::set3i(const VertexAttribute attrib, const int v0, const int v1, const int v2)
{
	if(m_vertexFormat.isAttributeEnabled(attrib) || m_vertexFormat.getElementCount(attrib) != 3)
	{
		((int*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[0] = v0;
		((int*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[1] = v1;
		((int*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[2] = v2;
	}
	else
	{
		LOG("Vertex format does not match");
	}
}

void Vertex::set3us(const VertexAttribute attrib, const ushort v0, const ushort v1, const ushort v2)
{
	if(m_vertexFormat.isAttributeEnabled(attrib) || m_vertexFormat.getElementCount(attrib) != 3)
	{
		((ushort*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[0] = v0;
		((ushort*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[1] = v1;
		((ushort*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[2] = v2;
	}
	else
	{
		LOG("Vertex format does not match");
	}
}

void Vertex::set3s(const VertexAttribute attrib, const short v0, const short v1, const short v2)
{
	if(m_vertexFormat.isAttributeEnabled(attrib) || m_vertexFormat.getElementCount(attrib) != 3)
	{
		((short*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[0] = v0;
		((short*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[1] = v1;
		((short*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[2] = v2;
	}
	else
	{
		LOG("Vertex format does not match");
	}
}

void Vertex::set3ub(const VertexAttribute attrib, const uchar v0, const uchar v1, const uchar v2)
{
	if(m_vertexFormat.isAttributeEnabled(attrib) || m_vertexFormat.getElementCount(attrib) != 3)
	{
		((uchar*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[0] = v0;
		((uchar*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[1] = v1;
		((uchar*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[2] = v2;
	}
	else
	{
		LOG("Vertex format does not match");
	}
}

void Vertex::set3b(const VertexAttribute attrib, const char v0, const char v1, const char v2)
{
	if(m_vertexFormat.isAttributeEnabled(attrib) || m_vertexFormat.getElementCount(attrib) != 3)
	{
		((char*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[0] = v0;
		((char*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[1] = v1;
		((char*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[2] = v2;
	}
	else
	{
		LOG("Vertex format does not match");
	}
}

void Vertex::set4f(const VertexAttribute attrib, const float v0, const float v1, const float v2, const float v3)
{
	if(m_vertexFormat.isAttributeEnabled(attrib) || m_vertexFormat.getElementCount(attrib) != 4)
	{
		((float*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[0] = v0;
		((float*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[1] = v1;
		((float*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[2] = v2;
		((float*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[3] = v3;
	}
	else
	{
		LOG("Vertex format does not match");
	}
}

void Vertex::set4ui(const VertexAttribute attrib, const uint v0, const uint v1, const uint v2, const uint v3)
{
	if(m_vertexFormat.isAttributeEnabled(attrib) || m_vertexFormat.getElementCount(attrib) != 4)
	{
		((uint*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[0] = v0;
		((uint*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[1] = v1;
		((uint*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[2] = v2;
		((uint*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[3] = v3;
	}
	else
	{
		LOG("Vertex format does not match");
	}
}

void Vertex::set4i(const VertexAttribute attrib, const int v0, const int v1, const int v2, const int v3)
{
	if(m_vertexFormat.isAttributeEnabled(attrib) || m_vertexFormat.getElementCount(attrib) != 4)
	{
		((int*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[0] = v0;
		((int*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[1] = v1;
		((int*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[2] = v2;
		((int*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[3] = v3;
	}
	else
	{
		LOG("Vertex format does not match");
	}
}

void Vertex::set4us(const VertexAttribute attrib, const ushort v0, const ushort v1, const ushort v2, const ushort v3)
{
	if(m_vertexFormat.isAttributeEnabled(attrib) || m_vertexFormat.getElementCount(attrib) != 4)
	{
		((ushort*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[0] = v0;
		((ushort*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[1] = v1;
		((ushort*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[2] = v2;
		((ushort*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[3] = v3;
	}
	else
	{
		LOG("Vertex format does not match");
	}
}

void Vertex::set4s(const VertexAttribute attrib, const short v0, const short v1, const short v2, const short v3)
{
	if(m_vertexFormat.isAttributeEnabled(attrib) || m_vertexFormat.getElementCount(attrib) != 4)
	{
		((short*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[0] = v0;
		((short*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[1] = v1;
		((short*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[2] = v2;
		((short*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[3] = v3;
	}
	else
	{
		LOG("Vertex format does not match");
	}
}

void Vertex::set4ub(const VertexAttribute attrib, const uchar v0, const uchar v1, const uchar v2, const uchar v3)
{
	if(m_vertexFormat.isAttributeEnabled(attrib) || m_vertexFormat.getElementCount(attrib) != 4)
	{
		((uchar*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[0] = v0;
		((uchar*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[1] = v1;
		((uchar*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[2] = v2;
		((uchar*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[3] = v3;
	}
	else
	{
		LOG("Vertex format does not match");
	}
}

void Vertex::set4b(const VertexAttribute attrib, const char v0, const char v1, const char v2, const char v3)
{
	if(m_vertexFormat.isAttributeEnabled(attrib) || m_vertexFormat.getElementCount(attrib) != 4)
	{
		((char*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[0] = v0;
		((char*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[1] = v1;
		((char*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[2] = v2;
		((char*) (m_vertexData + m_vertexFormat.getAttributeOffset(attrib)))[3] = v3;
	}
	else
	{
		LOG("Vertex format does not match");
	}
}

string Vertex::toString() const
{
	stringstream ss;
	ss << fixed << setprecision(2);
	for (int32 vertexAttributeIndex = 0; vertexAttributeIndex < (uint32)VertexAttribute::Max; ++vertexAttributeIndex)
	{
		VertexAttribute attrib = VertexAttribute(vertexAttributeIndex);
		if(m_vertexFormat.isAttributeEnabled(attrib))
		{
			switch(attrib)
			{
				case VertexAttribute::Position: ss << "\tPosition "; break;
				case VertexAttribute::Color:    ss << "\tColor "; break;
				case VertexAttribute::TexCoord: ss << "\tTex Coord "; break;
				case VertexAttribute::Normal:   ss << "\tNormal "; break;
			}

			const int32 elementCount = m_vertexFormat.getElementCount(attrib);

			switch (m_vertexFormat.getDatatype(attrib))
			{
				case Datatype::Float:  ss << "(float)"; break;
				case Datatype::Uint32: ss << "(uint32)"; break;
				case Datatype::Int32:  ss << "(int32)"; break;
				case Datatype::Uint16: ss << "(uint16)"; break;
				case Datatype::Int16:  ss << "(int16)"; break;
				case Datatype::Uint8:  ss << "(uint8)"; break;
				case Datatype::Int8:   ss << "(int8)"; break;
			}

			ss << ": [";
			for(int elementIndex = 0; elementIndex < elementCount; ++elementIndex)
			{
				const uint8 *const dataPtr = (m_vertexData + m_vertexFormat.getAttributeOffset(attrib));
				switch(m_vertexFormat.getDatatype(attrib))
				{
				case Datatype::Float:  ss << ((float*)dataPtr)[elementIndex]; break;
				case Datatype::Uint32: ss << ((uint32*)dataPtr)[elementIndex]; break;
				case Datatype::Int32:  ss << ((int32*)dataPtr)[elementIndex]; break;
				case Datatype::Uint16: ss << ((uint16*)dataPtr)[elementIndex]; break;
				case Datatype::Int16:  ss << ((int16*)dataPtr)[elementIndex]; break;
				case Datatype::Uint8:  ss << (int32)((uint8*)dataPtr)[elementIndex]; break;
				case Datatype::Int8:   ss << (int32)((int8*)dataPtr)[elementIndex]; break;
				}

				if (elementIndex < elementCount - 1)
				{
					ss << ", ";
				}
			}
			ss << "]" << endl;
		}
	}
	return ss.str();
}

VertexArray::VertexArray()
	: m_vertexCount(0)
	, m_vertexCapacity(0)
	, m_vertexFormat(VertexFormat::s_vtc)
	, m_vertexArrayData(nullptr)
	, m_vertexHandlesCache(nullptr)
{
}

VertexArray::VertexArray(const uint32 vertexCount)
	: m_vertexCount(0)
	, m_vertexCapacity(0)
	, m_vertexFormat(VertexFormat::s_vtc)
	, m_vertexArrayData(nullptr)
	, m_vertexHandlesCache(nullptr)
{
	resize(vertexCount);
}

VertexArray::VertexArray(const uint32 vertexCount, const VertexFormat& vertexFormat)
	: m_vertexCount(0)
	, m_vertexCapacity(0)
	, m_vertexFormat(vertexFormat)
	, m_vertexArrayData(nullptr)
	, m_vertexHandlesCache(nullptr)
{
	resize(vertexCount);
}

VertexArray::VertexArray(VertexArray&& other) noexcept
{
	m_vertexCount = other.m_vertexCount;
	m_vertexCapacity = other.m_vertexCapacity;
	m_vertexFormat = other.m_vertexFormat;
	m_vertexArrayData = other.m_vertexArrayData;
	m_vertexHandlesCache = other.m_vertexHandlesCache;
	other.m_vertexCount = 0;
	other.m_vertexCapacity = 0;
	other.m_vertexFormat = VertexFormat();
	other.m_vertexArrayData = nullptr;
	other.m_vertexHandlesCache = nullptr;
}

VertexArray::~VertexArray()
{
	delete[] m_vertexArrayData;
	delete[] m_vertexHandlesCache;
}

VertexArray& VertexArray::operator=(VertexArray&& other) noexcept
{
	delete[] m_vertexArrayData;
	delete[] m_vertexHandlesCache;
	m_vertexCount = other.m_vertexCount;
	m_vertexCapacity = other.m_vertexCapacity;
	m_vertexFormat = other.m_vertexFormat;
	m_vertexArrayData = other.m_vertexArrayData;
	m_vertexHandlesCache = other.m_vertexHandlesCache;
	other.m_vertexCount = 0;
	other.m_vertexCapacity = 0;
	other.m_vertexFormat = VertexFormat();
	other.m_vertexArrayData = nullptr;
	other.m_vertexHandlesCache = nullptr;
	return *this;
}

Vertex& VertexArray::operator[](const uint32 index)
{
	if (index > m_vertexCount)
	{
		LOG("Attempting to access a vertex outside the vertex array");
		static vector<uint8> zeroedData;
		if (zeroedData.size() < m_vertexFormat.getVertexSizeInBytes())
		{
			zeroedData.resize(m_vertexFormat.getVertexSizeInBytes());
		}
		static Vertex tmpVertex;
		tmpVertex.m_vertexData = zeroedData.data();
		tmpVertex.m_vertexFormat = m_vertexFormat;
		return tmpVertex;
	}
	return m_vertexHandlesCache[index];
}

uint32 VertexArray::getVertexCount() const
{
	return m_vertexCount;
}

VertexFormat VertexArray::getVertexFormat() const
{
	return m_vertexFormat;
}

uint8* VertexArray::getVertexData() const
{
	return m_vertexArrayData;
}

uint32 VertexArray::getVertexDataSize() const
{
	return m_vertexCount * m_vertexFormat.getVertexSizeInBytes();
}

void VertexArray::resize(const uint32 newVertexCount)
{
	if (m_vertexCapacity < newVertexCount)
	{
		// Temporarily store pointer to previous data
		const uint32 prevVertexDataSize = getVertexDataSize();
		const uint8* prevVertexArrayData = m_vertexArrayData;

		// Capacity needs to increase
		m_vertexCapacity = m_vertexCount = newVertexCount;
		const uint32 newVertexDataSize = getVertexDataSize();
		m_vertexArrayData = new uint8[newVertexDataSize];

		// Copy previous vertex data
		memcpy(m_vertexArrayData, prevVertexArrayData, prevVertexDataSize);

		// Clear previous data
		delete[] prevVertexArrayData;

		// Update vertex cache
		delete[] m_vertexHandlesCache;
		m_vertexHandlesCache = new Vertex[m_vertexCount];
		for (uint32 vertexIndex = 0; vertexIndex < m_vertexCount; ++vertexIndex)
		{
			m_vertexHandlesCache[vertexIndex].m_vertexData = m_vertexArrayData + (vertexIndex * m_vertexFormat.getVertexSizeInBytes());
			m_vertexHandlesCache[vertexIndex].m_vertexFormat = m_vertexFormat;
		}
	}
	else
	{
		// We already have the capacity, change vertex count only
		m_vertexCount = newVertexCount;
	}
}

VertexArray VertexArray::makeCopy() const
{
	VertexArray newVertexArray;
	newVertexArray.m_vertexFormat = m_vertexFormat;
	newVertexArray.resize(m_vertexCapacity);
	newVertexArray.m_vertexCount = m_vertexCount;
	memcpy(newVertexArray.m_vertexArrayData, m_vertexArrayData, getVertexDataSize());
	return move(newVertexArray);
}

string VertexArray::toString() const
{
	stringstream ss;
	for (uint32 vertexIndex = 0; vertexIndex < m_vertexCount; ++vertexIndex)
	{
		ss << "Vertex[" << vertexIndex << "]:" << endl;
		ss << m_vertexHandlesCache[vertexIndex].toString();
		ss << endl;
	}
	return ss.str();
}

END_SAUCE_NAMESPACE
