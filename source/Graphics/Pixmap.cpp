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
#include <FreeImage.h>

BEGIN_SAUCE_NAMESPACE

uint PixelFormat::getComponentCount() const
{
	switch (m_components)
	{
		case PixelComponents::R: return 1;
		case PixelComponents::Rg: return 2;
		case PixelComponents::Rgb: return 3;
		case PixelComponents::Rgba: return 4;
	}
	return 0;
}

uint PixelFormat::getDataTypeSizeInBytes() const
{
	switch (m_datatype)
	{
		case PixelDatatype::Uint32: case PixelDatatype::Int32: return 4;
		case PixelDatatype::Uint8: case PixelDatatype::Int8: return 1;
		case PixelDatatype::Float: return 4;
	}
	return 0;
}

uint PixelFormat::getPixelSizeInBytes() const
{
	return getComponentCount() * getDataTypeSizeInBytes();
}

Pixmap::Pixmap()
	: m_data(nullptr)
	, m_width(0)
	, m_height(0)
	, m_format()
{
}

Pixmap::Pixmap(const uint width, const uint height, const PixelFormat& format, const uint8_t* data)
	: m_width(width)
	, m_height(height)
	, m_format(format)
{
	const uint numBytes = m_width * m_height * m_format.getPixelSizeInBytes();
	if (numBytes > 0)
	{
		// Allocate pixel data
		m_data = new uint8[numBytes];
		if (data)
		{
			// Copy pixel data
			memcpy(m_data, data, numBytes);
		}
		else
		{
			memset(m_data, 0, numBytes);
		}
	}
	else
	{
		m_data = nullptr;
	}
}

Pixmap::Pixmap(const Pixmap& other)
	: m_width(other.m_width)
	, m_height(other.m_height)
	, m_format(other.m_format)
{
	if (other.m_data)
	{
		m_data = new uchar[m_width * m_height * m_format.getPixelSizeInBytes()];
		memcpy(m_data, other.m_data, m_width * m_height * m_format.getPixelSizeInBytes());
	}
	else
	{
		m_data = nullptr;
	}
}

Pixmap::Pixmap(Pixmap&& other) noexcept
{
	// Copy over values (shallow copy)
	m_width = other.m_width;
	m_height = other.m_height;
	m_format = other.m_format;
	m_data = other.m_data;

	// Invalidate other
	other.m_data = nullptr;
	other.m_width = 0;
	other.m_height = 0;
	other.m_format = PixelFormat();
}

Pixmap::~Pixmap()
{
	delete[] m_data;
}

Pixmap &Pixmap::operator=(Pixmap &other)
{
	m_width = other.m_width;
	m_height = other.m_height;
	m_format = other.m_format;

	// Free any existing data
	if (m_data)
	{
		delete[] m_data;
	}

	// Copy data from other
	if(other.m_data)
	{
		m_data = new uchar[m_width * m_height * m_format.getPixelSizeInBytes()];
		memcpy(m_data, other.m_data, m_width * m_height * m_format.getPixelSizeInBytes());
	}
	else
	{
		m_data = nullptr;
	}
	return *this;
}

const uchar *Pixmap::getData() const
{
	return m_data;
}

void Pixmap::setPremultipliedAlpha(const bool premultipliedAlpha)
{
	// TODO
}

uint Pixmap::getWidth() const
{
	return m_width;
}

uint Pixmap::getHeight() const
{
	return m_height;
}

PixelFormat Pixmap::getFormat() const
{
	return m_format;
}

bool Pixmap::isValid() const
{
	return m_data != nullptr && m_width > 0 && m_height > 0 &&
		m_format.getDataType() != PixelDatatype::Invalid &&
		m_format.getComponents() != PixelComponents::Invalid;
}

void Pixmap::getPixel(const uint x, const uint y, void *data) const
{
	if(x < m_width && y < m_height)
	{
		memcpy(data, m_data + (x + y * m_width) * m_format.getPixelSizeInBytes(), m_format.getPixelSizeInBytes());
	}
}

void Pixmap::setPixel(const uint x, const uint y, const void *data)
{
	if(x < m_width && y < m_height)
	{
		memcpy(m_data + (x + y * m_width) * m_format.getPixelSizeInBytes(), data, m_format.getPixelSizeInBytes());
	}
}

void Pixmap::flipY()
{
	uchar *pixel0 = new uchar[m_format.getPixelSizeInBytes()];
	uchar *pixel1 = new uchar[m_format.getPixelSizeInBytes()];
	for(int y0 = 0, y1 = m_height - 1; y0 < m_height / 2; y0++, y1--)
	{
		for(int x = 0; x < m_width; ++x)
		{
			getPixel(x, y0, pixel0);
			getPixel(x, y1, pixel1);
			setPixel(x, y1, pixel0);
			setPixel(x, y0, pixel1);
		}
	}
	delete[] pixel0;
	delete[] pixel1;
}

void Pixmap::fill(const void *data)
{
	for(uint y = 0; y < m_height; ++y)
	{
		for(uint x = 0; x < m_width; ++x)
		{
			memcpy(m_data + (x + y * m_width) * m_format.getPixelSizeInBytes(), data, m_format.getPixelSizeInBytes());
		}
	}
}

void Pixmap::clear()
{
	uchar *emptyPixel = new uchar[m_format.getPixelSizeInBytes()];
	memset(emptyPixel, 0, m_format.getPixelSizeInBytes());
	fill(emptyPixel);
	delete[] emptyPixel;
}

void Pixmap::saveToFile(string path) const
{
	if(m_format.getDataType() != PixelDatatype::Int8 && m_format.getDataType() != PixelDatatype::Uint8)
	{
		LOG("Cannot export a pixmap with a pixel data type different from byte or unsigned byte");
		return;
	}

	const uint32 numComponents = m_format.getComponentCount();
	const int32  pitch         = numComponents * m_width;
	const uint32 bpp           = numComponents * 8;

	// Convert pixmap to bitmap and save to PNG
	FIBITMAP *image = FreeImage_ConvertFromRawBits(m_data, m_width, m_height, pitch, bpp, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, false);
	FreeImage_FlipVertical(image);
	FreeImage_Save(FIF_PNG, image, path.c_str(), PNG_DEFAULT);
	FreeImage_Unload(image);
}

Pixmap Pixmap::loadFromFile(const string& imageFile)
{
	Pixmap newPixmap;

	// Check the file signature and deduce its format
	FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(imageFile.c_str(), 0);
	if (fif == FIF_UNKNOWN)
	{
		// Guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename(imageFile.c_str());
		if (fif == FIF_UNKNOWN)
		{
			LOG("Unable to determine format of image file \"%s\"", imageFile.c_str());
			return newPixmap;
		}
	}

	// Check that we can read this type of image file
	if (!FreeImage_FIFSupportsReading(fif))
	{
		LOG("Format of image file \"%s\" was recognized as \"%s\" but is unsupported", imageFile.c_str(), FreeImage_GetFormatFromFIF(fif));
		return newPixmap;
	}

	// Let's load the file
	FIBITMAP* bitmap = FreeImage_Load(fif, imageFile.c_str(), 0);
	if (!bitmap)
	{
		LOG("Error occured when loading image file \"%s\"; bitmap was nullptr", imageFile.c_str());
		return newPixmap;
	}

	// Read image data
	{
		// Convert bitmap to BGRA
		bitmap = FreeImage_ConvertTo32Bits(bitmap);
		FreeImage_FlipVertical(bitmap);

		// Create pixmap data
		newPixmap.m_format = PixelFormat(PixelComponents::Rgba, PixelDatatype::Uint8);
		newPixmap.m_width = FreeImage_GetWidth(bitmap);
		newPixmap.m_height = FreeImage_GetHeight(bitmap);
		assert(newPixmap.m_width >= 0 && newPixmap.m_height >= 0);
		newPixmap.m_data = new uchar[newPixmap.m_width * newPixmap.m_height * newPixmap.m_format.getPixelSizeInBytes()];

		// Copy pixels from bitmap
		uchar* pixels = (uchar*)FreeImage_GetBits(bitmap);
		for (int i = 0; i < newPixmap.m_width * newPixmap.m_height; i++)
		{
			// BGRA to RGBA
			newPixmap.m_data[i * 4 + 0] = pixels[i * 4 + 2];
			newPixmap.m_data[i * 4 + 1] = pixels[i * 4 + 1];
			newPixmap.m_data[i * 4 + 2] = pixels[i * 4 + 0];
			newPixmap.m_data[i * 4 + 3] = pixels[i * 4 + 3];
		}

		FreeImage_Unload(bitmap);
	}

	return std::move(newPixmap);
}

ByteStreamOut& operator<<(ByteStreamOut& out, const Pixmap& pixmap)
{
	out << pixmap.m_width;
	out << pixmap.m_height;
	out << (uint32)pixmap.m_format.getComponents();
	out << (uint32)pixmap.m_format.getDataType();

	vector<uint8> dataArray;
	const int32 dataSize = pixmap.m_width * pixmap.m_height * pixmap.m_format.getPixelSizeInBytes();
	dataArray.resize(dataSize);
	memcpy(dataArray.data(), pixmap.m_data, dataSize);

	out << dataArray;
	return out;
}

ByteStreamIn& operator>>(ByteStreamIn& in, Pixmap& pixmap)
{
	in >> pixmap.m_width;
	in >> pixmap.m_height;

	uint32 components, datatype;
	in >> components;
	in >> datatype;
	pixmap.m_format = PixelFormat((PixelComponents)components, (PixelDatatype)datatype);

	vector<uint8> dataArray;
	in >> dataArray;
	delete[] pixmap.m_data;
	pixmap.m_data = new uint8[dataArray.size()];
	memcpy(pixmap.m_data, dataArray.data(), dataArray.size());

	return in;
}

END_SAUCE_NAMESPACE

