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

#include <Sauce/Math.h>
#include <Sauce/Math/Vector.h>
#include <Sauce/Graphics.h>
#include <Sauce/Graphics/Texture.h>

#include <ft2build.h>
#include FT_FREETYPE_H

BEGIN_SAUCE_NAMESPACE

FT_Library   g_library;
const uint32 g_defaultFontSize = 128;

//--------------------------------------------------------------
// FontRenderingSystem::initialize()
//--------------------------------------------------------------
bool FontRenderingSystem::initialize()
{
	FT_Error error = FT_Init_FreeType(&g_library);
	if (error)
	{
		LOG("Failed to initialize the FontRenderingSystem (error string: \"%s\")", FT_Error_String(error));
		return false;
	}

	return true;
}

void FontRenderingSystem::free()
{
	FT_Done_FreeType(g_library);
}

//--------------------------------------------------------------
// FontRenderer implementation
//--------------------------------------------------------------
class FontRendererImpl : public FontRenderer
{
public:
	FontRendererImpl()
		: m_face(nullptr)
		, m_fontSize(0)
		, m_fontPadding(16)
		, m_sdfRadius(6)
		, m_sdfAtlas(nullptr)
		, m_subdivisionsPerSDFPixel(4)
	{
	}

	~FontRendererImpl()
	{
		FT_Done_Face(m_face);
		delete m_sdfAtlas;
	}

	virtual bool initialize(const string& fontFilePath)
	{
		FT_Error error = FT_New_Face(g_library, fontFilePath.c_str(), 0, &m_face);
		if (error)
		{
			LOG("Failed create font typeface (error string: \"%s\")", FT_Error_String(error));
			return false;
		}

		// Initialize font with default font size
		if (!setFontSize(g_defaultFontSize))
		{
			return false;
		}
		return true;
	}

	bool setFontSize(uint32 fontSize)
	{
		FT_Error error = FT_Set_Char_Size(m_face, 0, fontSize * 64, 0, 96);
		if (error)
		{
			LOG("Failed create set font size (error string: \"%s\")", FT_Error_String(error));
			return false;
		}
		m_fontSize = fontSize;
		updateGlyphAtlas();
		return true;
	}

	void drawString(GraphicsContext* context, const string& str)
	{
		context->setTexture(m_sdfAtlasTexture);
		context->drawRectangle(Vector2F(0, 0), m_sdfAtlasTexture->getSize() * 2);
		context->setTexture(0);
	}

private:
	struct GlyphDesc
	{
		uint64 charcode;
		uint32 glyphIndex;
		uint32 x;
		uint32 y;
		uint32 width;
		uint32 height;
	};

	void updateGlyphAtlas()
	{
		static uint32 MAX_ATLAS_WIDTH = 4096;

		// Calculate the extents of the resulting pixmap
		// and gather valid glyphs
		Vector2I extents;
		{
			// We will generate glyphs for ASCII symbols
			static uint8 NUM_GLYPHS = 0xFF;
			m_charcodeToGlyph.reserve(NUM_GLYPHS);
			Vector2I currentOffset;
			uint32 maxGlyphHeight = 0;
			for (uint64 charcode = 0; charcode <= NUM_GLYPHS; ++charcode)
			{
				const uint32 glyphIndex = FT_Get_Char_Index(m_face, charcode);
				if (glyphIndex <= 0)
				{
					// Skip invalid glyphs
					continue;
				}

				FT_Error error = FT_Load_Glyph(m_face, glyphIndex, FT_LOAD_DEFAULT);
				if (error)
				{
					LOG("Failed to load glyph for charcode=%l (error string: \"%s\")", charcode, FT_Error_String(error));
					continue;
				}

				const FT_Glyph_Metrics metrics = m_face->glyph->metrics;
				const uint32 glyphWidth = (metrics.width / 64) + m_fontPadding;
				const uint32 glyphHeight = (metrics.height / 64) + m_fontPadding;

				// If we exceed the max width, expand downwards
				if (currentOffset.x + glyphWidth >= MAX_ATLAS_WIDTH)
				{
					currentOffset.x = 0;
					currentOffset.y += maxGlyphHeight;
					maxGlyphHeight = 0;
				}

				// Add glyph descriptor
				{
					GlyphDesc glyphDesc;
					glyphDesc.charcode = charcode;
					glyphDesc.glyphIndex = glyphIndex;
					glyphDesc.x = currentOffset.x + m_fontPadding / 2;
					glyphDesc.y = currentOffset.y + m_fontPadding / 2;
					glyphDesc.width = glyphWidth;
					glyphDesc.height = glyphHeight;
					m_charcodeToGlyph[charcode] = glyphDesc;
				}

				// Update offsets and extents
				currentOffset.x += glyphWidth;
				maxGlyphHeight = math::maximum(maxGlyphHeight, glyphHeight);
				extents.x = math::maximum(extents.x, currentOffset.x);
				extents.y = math::maximum(extents.y, currentOffset.y + maxGlyphHeight);
			}
		}

		// Ceil to pow2
		extents.x = math::ceilPow2(extents.x);
		extents.y = math::ceilPow2(extents.y);

		// Render characters to atlas
		uint8* glyphAtlasData = new uint8[extents.x * extents.y];
		memset(glyphAtlasData, 0, extents.x * extents.y);
		for (const pair<uint64, GlyphDesc>& itr : m_charcodeToGlyph)
		{
			const GlyphDesc& glyphDesc = itr.second;
			FT_Load_Glyph(m_face, glyphDesc.glyphIndex, FT_LOAD_DEFAULT);
			FT_Error error = FT_Render_Glyph(m_face->glyph, FT_RENDER_MODE_MONO);
			if (error)
			{
				LOG("Failed to render glyph for glyphIndex=%i (error string: \"%s\")", glyphDesc.glyphIndex, FT_Error_String(error));
				continue;
			}
			drawBitmapToAtlas(glyphAtlasData, extents.x, &m_face->glyph->bitmap, glyphDesc.x, glyphDesc.y);
		}

		// Create sdf map
		const int32 sdfMapSizeX = extents.x / m_subdivisionsPerSDFPixel;
		const int32 sdfMapSizeY = extents.y / m_subdivisionsPerSDFPixel;
		m_sdfAtlas = new Pixmap(sdfMapSizeX, sdfMapSizeY, PixelFormat(PixelFormat::R, PixelFormat::UNSIGNED_BYTE));
		{
			for (int32 sdfMapY = 0; sdfMapY < sdfMapSizeY; ++sdfMapY)
			{
				for (int32 sdfMapX = 0; sdfMapX < sdfMapSizeX; ++sdfMapX)
				{
					// We first check if this SDF pixel contains a contour
					{
						bool isContour = false;
						uint32 x = sdfMapX * m_subdivisionsPerSDFPixel;
						uint32 y = sdfMapY * m_subdivisionsPerSDFPixel;
						const bool isFirstPixelInside = glyphAtlasData[x + y * extents.x] != 0;

						// For each pixel in the high-res image contained within this SDF pixel
						for (int32 subPixelY = 0; subPixelY < m_subdivisionsPerSDFPixel && !isContour; ++subPixelY)
						{
							for (int32 subPixelX = 0; subPixelX < m_subdivisionsPerSDFPixel; ++subPixelX)
							{
								// Search in a radius for a pixel in an opposite state
								x = sdfMapX * m_subdivisionsPerSDFPixel + subPixelX;
								y = sdfMapY * m_subdivisionsPerSDFPixel + subPixelY;
								const bool isPixelInside = glyphAtlasData[x + y * extents.x] != 0;
								if (isFirstPixelInside != isPixelInside)
								{
									isContour = true;
									break;
								}
							}
						}

						if (isContour)
						{
							// Write 0.5 if this SDF pixel contains a contour
							const uint8 sdfPixelValue = 127;
							m_sdfAtlas->setPixel(sdfMapX, sdfMapY, &sdfPixelValue);
							continue;
						}
					}

					// Iterate pixels at the edge of the SDF cell and find min signed distance
					{
						float signedDistanceSqMin = numeric_limits<float>::max();

						// For each pixel in the high-res image contained within this SDF pixel
						for (int32 subPixelY = 0; subPixelY < m_subdivisionsPerSDFPixel; ++subPixelY)
						{
							for (int32 subPixelX = 0; subPixelX < m_subdivisionsPerSDFPixel; ++subPixelX)
							{
								// Skip center pixels
								if ((subPixelX != 0 && subPixelX != m_subdivisionsPerSDFPixel - 1) &&
									(subPixelY != 0 && subPixelY != m_subdivisionsPerSDFPixel - 1))
								{
									continue;
								}

								// Search in a radius for a pixel in an opposite state
								const uint32 x = sdfMapX * m_subdivisionsPerSDFPixel + subPixelX;
								const uint32 y = sdfMapY * m_subdivisionsPerSDFPixel + subPixelY;

								// Check if we are starting from the inside or the outside
								const bool isSubPixelInside = glyphAtlasData[x + y * extents.x] != 0;
								float distanceSq = m_sdfRadius * m_sdfRadius;

								for (int32 offsetY = -m_sdfRadius; offsetY <= m_sdfRadius; ++offsetY)
								{
									for (int32 offsetX = -m_sdfRadius; offsetX <= m_sdfRadius; ++offsetX)
									{
										if ((x + offsetX < 0 || x + offsetX >= extents.x) ||
											(y + offsetY < 0 || y + offsetY >= extents.y))
										{
											continue;
										}

										// Skip pixel if it is further away than our current min distance
										const float currentDistanceSq = offsetX * offsetX + offsetY * offsetY;
										if (currentDistanceSq > distanceSq)
										{
											continue;
										}

										// Update distance if pixel is in opposite state
										const bool isPixelInside = glyphAtlasData[(x + offsetX) + (y + offsetY) * extents.x] != 0;
										if (isPixelInside != isSubPixelInside)
										{
											distanceSq = currentDistanceSq;
										}
									}
								}

								// Update the smallest distance for this SDF cell if the magnitude of the
								// squared distance for this pixel is less than the magnitude of the
								// current min signed distance squared
								if (distanceSq < abs(signedDistanceSqMin))
								{
									signedDistanceSqMin = distanceSq * (isSubPixelInside ? 1 : -1);
								}
							}
						}

						const float signedDistanceNorm = signedDistanceSqMin / float(m_sdfRadius * m_sdfRadius); // [-1, +1]
						const float distanceNorm       = ((signedDistanceNorm + 1.0f) / 2.0f);                   // [+0, +1]
						const uint8 sdfPixelValue      = distanceNorm * 255.5f;                                  // [+0, +255]
						m_sdfAtlas->setPixel(sdfMapX, sdfMapY, &sdfPixelValue);
					}
				}
			}
		}
		m_sdfAtlasTexture = shared_ptr<Texture2D>(Game::Get()->getWindow()->getGraphicsContext()->createTexture(*m_sdfAtlas));
		m_sdfAtlasTexture->setFiltering(Texture2D::LINEAR);
		//m_sdfAtlasTexture->setWrapping(Texture2D::CLAMP_TO_EDGE);

		delete[] glyphAtlasData;
	}

	void drawBitmapToAtlas(uint8* glyphAtlasData, int32 glyphAtlasWidth, FT_Bitmap* bitmap, const int32 offsetX, const int32 offsetY)
	{
		assert(bitmap->pixel_mode == FT_PIXEL_MODE_MONO);
		for (int32 y = 0; y < bitmap->rows; ++y)
		{
			for (int32 x = 0; x < bitmap->width; ++x)
			{
				const uint32 byteOffset = y * bitmap->pitch + x / 8; // Which byte is this pixel stored in?
				const uint32 bitmask = 0x80 >> (x % 8);              // Which bit in the byte represents the current pixel?
				const uint8  pixelValue = ((bitmap->buffer[byteOffset] & bitmask) != 0) ? 255 : 0;
				glyphAtlasData[(offsetX + x) + (offsetY + y) * glyphAtlasWidth] = pixelValue;
			}
		}
	}

	FT_Face m_face;
	int32 m_fontSize;
	int32 m_fontPadding;
	Pixmap* m_sdfAtlas;
	int32 m_sdfRadius;
	shared_ptr<Texture2D> m_sdfAtlasTexture;
	int32 m_subdivisionsPerSDFPixel;

	unordered_map<uint64, GlyphDesc> m_charcodeToGlyph;
};

//--------------------------------------------------------------
// FontRenderingSystem::createFontRenderer()
//--------------------------------------------------------------
FontRenderer* FontRenderingSystem::createFontRenderer(const string& fontFilePath)
{
	FontRenderer* fontRenderer = new FontRendererImpl();
	if (!fontRenderer->initialize(fontFilePath))
	{
		delete fontRenderer;
		return nullptr;
	}
	return fontRenderer;
}

END_SAUCE_NAMESPACE
