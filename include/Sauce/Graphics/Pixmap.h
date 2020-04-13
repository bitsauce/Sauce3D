// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#pragma once

#include <Sauce/Common.h>

BEGIN_SAUCE_NAMESPACE

enum class PixelComponents : uint32
{
	R,
	RG,
	RGB,
	RGBA,
	INVALID_COMPONENTS
};

enum class PixelDatatype : uint32
{
	INT,
	UNSIGNED_INT,
	BYTE,
	UNSIGNED_BYTE,
	FLOAT,
	INVALID_DATA_TYPE
};

class SAUCE_API PixelFormat
{
public:
	PixelFormat()
		: m_components(PixelComponents::INVALID_COMPONENTS)
		, m_datatype(PixelDatatype::INVALID_DATA_TYPE)
	{
	}

	PixelFormat(PixelComponents components, PixelDatatype datatype)
		: m_components(components)
		, m_datatype(datatype)
	{
	}

	PixelComponents getComponents() const { return m_components; }
	PixelDatatype getDataType() const { return m_datatype; }

	uint getComponentCount() const;
	uint getDataTypeSizeInBytes() const;
	uint getPixelSizeInBytes() const;

private:
	PixelComponents m_components;
	PixelDatatype m_datatype;
};

class SAUCE_API Pixmap
{
public:
	Pixmap();
	Pixmap(const uint width, const uint height, const PixelFormat& format, const uint8_t* data=nullptr);
	Pixmap(const Pixmap& other);
	Pixmap(Pixmap&& other) noexcept;
	~Pixmap();

	Pixmap &operator=(Pixmap &other);

	uint getWidth() const;
	uint getHeight() const;
	PixelFormat getFormat() const;
	bool isValid() const;

	void getPixel(const uint x, const uint y, void *data) const;
	void setPixel(const uint x, const uint y, const void *data);

	void flipY();

	void fill(const void* data);
	void clear();

	const uchar* getData() const;

	void setPremultipliedAlpha(const bool premultipliedAlpha);

	void saveToFile(string path) const;
	static Pixmap loadFromFile(const string& imageFile);

private:
	uchar* m_data;
	uint   m_width;
	uint   m_height;
	PixelFormat m_format;
};

END_SAUCE_NAMESPACE
