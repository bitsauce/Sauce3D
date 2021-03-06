// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#pragma once

#include <Sauce/Common.h>
#include <Sauce/Utils/FileSystemUtils.h>

BEGIN_SAUCE_NAMESPACE

enum class PixelComponents : uint32
{
	R,
	Rg,
	Rgb,
	Rgba,
	Invalid
};

enum class PixelDatatype : uint32
{
	Int8,
	Uint8,
	Int32,
	Uint32,
	Float,
	Invalid
};

class SAUCE_API PixelFormat
{
public:
	PixelFormat()
		: m_components(PixelComponents::Invalid)
		, m_datatype(PixelDatatype::Invalid)
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

	friend ByteStreamOut& operator<<(ByteStreamOut& out, const Pixmap& pixmap);
	friend ByteStreamIn& operator>>(ByteStreamIn& in, Pixmap& pixmap);

private:
	uchar* m_data;
	uint   m_width;
	uint   m_height;
	PixelFormat m_format;
};

END_SAUCE_NAMESPACE
