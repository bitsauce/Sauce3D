//     _____                        ______             _            
//    / ____|                      |  ____|           (_)           
//   | (___   __ _ _   _  ___ ___  | |__   _ __   __ _ _ _ __   ___ 
//    \___ \ / _` | | | |/ __/ _ \ |  __| | '_ \ / _` | | '_ \ / _ \
//    ____) | (_| | |_| | (_|  __/ | |____| | | | (_| | | | | |  __/
//   |_____/ \__,_|\__,_|\___\___| |______|_| |_|\__, |_|_| |_|\___|
//                                                __/ |             
//                                               |___/              
// Made by Marcus "Bitsauce" Loo Vergara
// 2011-2018 (C)

#include <Sauce/Common.h>
#include <Sauce/Graphics.h>
#include <FreeImage.h>

BEGIN_SAUCE_NAMESPACE

uint PixelFormat::getComponentCount() const
{
	switch (m_components) {
	case R: return 1;
	case RG: return 2;
	case RGB: return 3;
	case RGBA: return 4;
	}
	return 0;
}

uint PixelFormat::getDataTypeSizeInBytes() const
{
	switch (m_dataType)
	{
	case UNSIGNED_INT: case INT: return sizeof(GLint);
	case UNSIGNED_BYTE: case BYTE: return sizeof(GLbyte);
	case FLOAT: return sizeof(GLfloat);
	}
	return 0;
}

uint PixelFormat::getPixelSizeInBytes() const
{
	return getComponentCount() * getDataTypeSizeInBytes();
}

Pixmap::Pixmap(const PixelFormat &format) :
	m_width(0),
	m_height(0),
	m_data(0),
	m_format(format)
{
}

Pixmap::Pixmap(const uint width, const uint height, const void *data, const PixelFormat &format) :
	m_width(width),
	m_height(height),
	m_format(format)
{
	// Copy pixels
	if(width >= 0 && height >= 0)
	{
		m_data = new uchar[width * height * m_format.getPixelSizeInBytes()];
		memcpy(m_data, data, width * height * m_format.getPixelSizeInBytes());
	}
	else
	{
		m_data = 0;
	}
}

Pixmap::Pixmap(const uint width, const uint height, const PixelFormat &format) :
	m_width(width),
	m_height(height),
	m_format(format)
{
	// Create empty pixmap
	if(width >= 0 && height >= 0)
	{
		m_data = new uchar[width * height * m_format.getPixelSizeInBytes()];
		memset(m_data, 0, width * height * m_format.getPixelSizeInBytes());
	}
	else
	{
		m_data = 0;
	}
}

Pixmap::Pixmap(const Pixmap &other)
{
	m_width = other.m_width;
	m_height = other.m_height;
	m_format = other.m_format;
	if(other.m_data)
	{
		m_data = new uchar[m_width * m_height * m_format.getPixelSizeInBytes()];
		memcpy(m_data, other.m_data, m_width * m_height * m_format.getPixelSizeInBytes());
	}
	else
	{
		m_data = 0;
	}
}

Pixmap::Pixmap(const string &imageFile, const bool premultiplyAlpha) :
	m_format(PixelFormat::RGBA, PixelFormat::UNSIGNED_BYTE),
	m_data(nullptr),
	m_width(0),
	m_height(0)
{
	// Check the file signature and deduce its format
	FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(imageFile.c_str(), 0);
	if(fif == FIF_UNKNOWN)
	{
		// Guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename(imageFile.c_str());
	}

	// Check that the plugin has reading capabilities...
	FIBITMAP *bitmap = nullptr;
	if(fif != FIF_UNKNOWN && FreeImage_FIFSupportsReading(fif))
	{
		// Let's load the file
		bitmap = FreeImage_Load(fif, imageFile.c_str(), 0);
	}

	if(bitmap)
	{
		// Convert bitmap to BGRA
		bitmap = FreeImage_ConvertTo32Bits(bitmap);
		FreeImage_FlipVertical(bitmap);
		if(premultiplyAlpha) FreeImage_PreMultiplyWithAlpha(bitmap);

		// Create pixmap data
		m_width = FreeImage_GetWidth(bitmap), m_height = FreeImage_GetHeight(bitmap);
		if(m_width >= 0 && m_height >= 0)
		{
			m_data = new uchar[m_width * m_height * m_format.getPixelSizeInBytes()];
		}

		// Copy pixels from bitmap
		uchar *pixels = (uchar*)FreeImage_GetBits(bitmap);
		for(int i = 0; i < m_width * m_height; i++)
		{
			m_data[i * 4 + 0] = pixels[i * 4 + 2];
			m_data[i * 4 + 1] = pixels[i * 4 + 1];
			m_data[i * 4 + 2] = pixels[i * 4 + 0];
			m_data[i * 4 + 3] = pixels[i * 4 + 3];
		}

		FreeImage_Unload(bitmap);
	}
}

/*Pixmap::Pixmap &operator=(Pixmap &other)
{
	swap(m_data, other.m_data);
	swap(m_width, other.m_width);
	swap(m_height, other.m_height);
	return *this;
}*/

Pixmap::~Pixmap()
{
	delete[] m_data;
}

Pixmap &Pixmap::operator=(Pixmap &other)
{
	m_width = other.m_width;
	m_height = other.m_height;
	m_format = other.m_format;
	if(other.m_data)
	{
		m_data = new uchar[m_width * m_height * m_format.getPixelSizeInBytes()];
		memcpy(m_data, other.m_data, m_width * m_height * m_format.getPixelSizeInBytes());
	}
	else
	{
		m_data = 0;
	}
	return *this;
}

const uchar *Pixmap::getData() const
{
	return m_data;
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
		memcpy(m_data + (x + y*m_width) * m_format.getPixelSizeInBytes(), data, m_format.getPixelSizeInBytes());
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
			memcpy(m_data + (x + y*m_width) * m_format.getPixelSizeInBytes(), data, m_format.getPixelSizeInBytes());
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

void Pixmap::exportToFile(string path) const
{
	if(m_format.getDataType() != PixelFormat::BYTE && m_format.getDataType() != PixelFormat::UNSIGNED_BYTE)
	{
		LOG("Cannot export image with a pixel data type different from byte or unsigned byte");
		return;
	}

	// Convert pixmap to surface and export as a PNG image
	/*SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(m_data, m_width, m_height, 32, m_width * 4, R_MASK, G_MASK, B_MASK, A_MASK);
	util::toAbsoluteFilePath(path);
	IMG_SavePNG(surface, path.c_str());
	SDL_FreeSurface(surface);*/

	FIBITMAP *image = FreeImage_ConvertFromRawBits(m_data, m_width, m_height, m_width * 4, 32, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, false);
	FreeImage_Save(FIF_PNG, image, path.c_str(), PNG_DEFAULT);
	FreeImage_Unload(image);
}

END_SAUCE_NAMESPACE
