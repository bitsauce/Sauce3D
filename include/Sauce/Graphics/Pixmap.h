#ifndef SAUCE_PIXMAP_H
#define SAUCE_PIXMAP_H

#include <Sauce/Common.h>

BEGIN_SAUCE_NAMESPACE

class SAUCE_API PixelFormat
{
public:
	enum Components
	{
		R,
		RG,
		RGB,
		RGBA,
		INVALID_COMPONENTS
	};

	enum DataType
	{
		INT,
		UNSIGNED_INT,
		BYTE,
		UNSIGNED_BYTE,
		FLOAT,
		INVALID_DATA_TYPE
	};

	PixelFormat()
		: m_components(Components::INVALID_COMPONENTS)
		, m_dataType(DataType::INVALID_DATA_TYPE)
	{
	}

	PixelFormat(Components components, DataType dataType)
		: m_components(components)
		, m_dataType(dataType)
	{
	}

	Components getComponents() const { return m_components; }
	DataType getDataType() const { return m_dataType; }

	uint getComponentCount() const;
	uint getDataTypeSizeInBytes() const;
	uint getPixelSizeInBytes() const;

private:
	Components m_components;
	DataType m_dataType;
};

class SAUCE_API Pixmap
{
public:
	Pixmap();
	Pixmap(const uint width, const uint height, const PixelFormat& format, const uint8_t* data=nullptr);
	Pixmap(const Pixmap& other);
	Pixmap(Pixmap&& other);
	~Pixmap();

	Pixmap &operator=(Pixmap &other);

	uint getWidth() const;
	uint getHeight() const;
	PixelFormat getFormat() const;
	bool isValid() const;

	void getPixel(const uint x, const uint y, void *data) const;
	void setPixel(const uint x, const uint y, const void *data);

	void flipY();

	void fill(const void *data);
	void clear();

	const uchar *getData() const;

	void setPremultipliedAlpha(const bool premultipliedAlpha);

	void saveToFile(string path) const;
	static Pixmap loadFromFile(const string& imageFile);

private:
	uchar *m_data;
	uint m_width;
	uint m_height;
	PixelFormat m_format;
};

END_SAUCE_NAMESPACE

#endif // SAUCE_PIXMAP_H
