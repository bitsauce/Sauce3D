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

#include <Sauce/Math.h>
#include <Sauce/Math/Vector.h>
#include <Sauce/Graphics.h>
#include <Sauce/Graphics/Texture.h>

#include <ft2build.h>
#include FT_FREETYPE_H

BEGIN_SAUCE_NAMESPACE

FT_Library   g_library;
const uint32 g_defaultFontSize = 128;
VertexFormat g_fontVertexFormat;

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

	// Set font vertex format
	g_fontVertexFormat.set(VertexAttribute::VERTEX_POSITION, 2, Datatype::SAUCE_FLOAT);
	g_fontVertexFormat.set(VertexAttribute::VERTEX_TEX_COORD, 2, Datatype::SAUCE_FLOAT);
	g_fontVertexFormat.set(VertexAttribute::VERTEX_COLOR, 4, Datatype::SAUCE_UBYTE);

	return true;
}

void FontRenderingSystem::free()
{
	FT_Done_FreeType(g_library);
}

//--------------------------------------------------------------
// FontRenderer implementation
//--------------------------------------------------------------
// TODO: Should support vertical text rendering
// TODO: Should support boxed text rendering
// TODO: Text coloring would be nice I suppose
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

	virtual bool initialize(const string& fontFilePath) override
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

	bool setFontSize(uint32 fontSize) override
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

	void drawString(GraphicsContext* context, const string& str, const float startX, const float startY,
		const float scale, const float rotation, const TextAlignment alignment) override
	{
		const int32 numChars = str.length();

		Vector2F extentsOfString;
		for (int32 i = 0; i < numChars; ++i)
		{
			// Find glyph desc
			GlyphDesc glyphDesc;
			{
				const uint64 charcode = str[i];
				unordered_map<uint64, GlyphDesc>::iterator itr = m_charcodeToGlyph.find(charcode);
				if (itr == m_charcodeToGlyph.end())
				{
					LOG("Tried to render glyph with charcode '%s', but no maching glyph descriptor was found", charcode);
					continue;
				}
				glyphDesc = itr->second;
			}

			extentsOfString += glyphDesc.advance;
		}

		// TODO: Should cache rendered string
		//m_cachedRenderedText = context->createRenderTarget(extentsOfString.x, extentsOfString.y);
		
		Vertex* vertices = g_fontVertexFormat.createVertices(numChars * 4);
		uint32* indices = new uint32[numChars * 6];
		Vector2F currentPos = Vector2F(0.0f, 0.0f);

		if (alignment == TextAlignment::Centered)
		{
			currentPos.x = -extentsOfString.x / 2.0f;
		}
		else if (alignment == TextAlignment::Right)
		{
			currentPos.x = -extentsOfString.x;
		}

		for (int32 i = 0; i < numChars; ++i)
		{
			// Find glyph desc
			GlyphDesc glyphDesc;
			{
				const uint64 charcode = str[i];
				unordered_map<uint64, GlyphDesc>::iterator itr = m_charcodeToGlyph.find(charcode);
				if (itr == m_charcodeToGlyph.end())
				{
					LOG("Tried to render glyph with charcode '%s', but no maching glyph descriptor was found", charcode);
					continue;
				}
				glyphDesc = itr->second;
			}

			const Vector2F currentTL = currentPos + glyphDesc.pixelDrawOffset;
			const Vector2F currentBR = currentPos + glyphDesc.pixelSize + glyphDesc.pixelDrawOffset;
			vertices[i * 4 + 0].set2f(VertexAttribute::VERTEX_POSITION, currentTL.x, currentTL.y);
			vertices[i * 4 + 0].set2f(VertexAttribute::VERTEX_TEX_COORD, glyphDesc.uv0.x, glyphDesc.uv0.y);
			vertices[i * 4 + 0].set4ub(VertexAttribute::VERTEX_COLOR, 255, 255, 255, 255);

			vertices[i * 4 + 1].set2f(VertexAttribute::VERTEX_POSITION, currentBR.x, currentTL.y);
			vertices[i * 4 + 1].set2f(VertexAttribute::VERTEX_TEX_COORD, glyphDesc.uv1.x, glyphDesc.uv0.y);
			vertices[i * 4 + 1].set4ub(VertexAttribute::VERTEX_COLOR, 255, 255, 255, 255);

			vertices[i * 4 + 2].set2f(VertexAttribute::VERTEX_POSITION, currentTL.x, currentBR.y);
			vertices[i * 4 + 2].set2f(VertexAttribute::VERTEX_TEX_COORD, glyphDesc.uv0.x, glyphDesc.uv1.y);
			vertices[i * 4 + 2].set4ub(VertexAttribute::VERTEX_COLOR, 255, 255, 255, 255);

			vertices[i * 4 + 3].set2f(VertexAttribute::VERTEX_POSITION, currentBR.x, currentBR.y);
			vertices[i * 4 + 3].set2f(VertexAttribute::VERTEX_TEX_COORD, glyphDesc.uv1.x, glyphDesc.uv1.y);
			vertices[i * 4 + 3].set4ub(VertexAttribute::VERTEX_COLOR, 255, 255, 255, 255);

			indices[i * 6 + 0] = i * 4 + 0;
			indices[i * 6 + 1] = i * 4 + 2;
			indices[i * 6 + 2] = i * 4 + 1;

			indices[i * 6 + 3] = i * 4 + 3;
			indices[i * 6 + 4] = i * 4 + 1;
			indices[i * 6 + 5] = i * 4 + 2;

			currentPos += glyphDesc.advance;
		}

		context->setTexture(m_sdfAtlasTexture);
		Matrix4 textTransform;
		textTransform.scale(scale);
		textTransform.rotateZ(rotation);
		textTransform.translate(startX, startY, 0.0f);
		context->pushMatrix(textTransform);
		context->drawIndexedPrimitives(PrimitiveType::PRIMITIVE_TRIANGLES, vertices, numChars * 4, indices, numChars * 6);
		context->popMatrix();
		context->setTexture(nullptr);

		delete[] vertices;
		delete[] indices;
	}

private:
	struct GlyphDesc
	{
		uint64 charcode;
		uint32 glyphIndex;
		Vector2I pixelSize;
		Vector2I pixelPos;
		Vector2I pixelDrawOffset;
		Vector2F advance;
		Vector2F uv0;
		Vector2F uv1;
	};

	void updateGlyphAtlas()
	{
		// TODO: Should cache glyph atlases as they're pretty slow to create
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
					glyphDesc.charcode   = charcode;
					glyphDesc.glyphIndex = glyphIndex;
					glyphDesc.pixelPos   = currentOffset;
					glyphDesc.pixelSize.set(glyphWidth, glyphHeight);
					glyphDesc.advance.set(m_face->glyph->advance.x / 64, m_face->glyph->advance.y / 64);
					glyphDesc.pixelDrawOffset.set(m_face->glyph->metrics.horiBearingX / 64, -m_face->glyph->metrics.horiBearingY / 64);
					

					m_charcodeToGlyph[charcode] = glyphDesc;
				}

				// Update offsets and extents
				currentOffset.x += glyphWidth + m_fontPadding;
				maxGlyphHeight = math::maximum(maxGlyphHeight, glyphHeight + m_fontPadding);
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
		for (unordered_map<uint64, GlyphDesc>::iterator itr = m_charcodeToGlyph.begin(); itr != m_charcodeToGlyph.end(); ++itr)
		{
			GlyphDesc& glyphDesc = itr->second;
			FT_Load_Glyph(m_face, glyphDesc.glyphIndex, FT_LOAD_DEFAULT);
			FT_Error error = FT_Render_Glyph(m_face->glyph, FT_RENDER_MODE_MONO);
			if (error)
			{
				LOG("Failed to render glyph for glyphIndex=%i (error string: \"%s\")", glyphDesc.glyphIndex, FT_Error_String(error));
				continue;
			}
			drawBitmapToAtlas(glyphAtlasData, extents.x, &m_face->glyph->bitmap, glyphDesc.pixelPos.x + m_fontPadding / 2, glyphDesc.pixelPos.y + m_fontPadding / 2);
			glyphDesc.uv0 = Vector2F(glyphDesc.pixelPos) / Vector2F(extents);
			glyphDesc.uv1 = Vector2F(glyphDesc.pixelPos + glyphDesc.pixelSize) / Vector2F(extents);
		}

		// Create sdf map
		const int32 sdfMapSizeX = extents.x / m_subdivisionsPerSDFPixel;
		const int32 sdfMapSizeY = extents.y / m_subdivisionsPerSDFPixel;
		m_sdfAtlas = new Pixmap(sdfMapSizeX, sdfMapSizeY, PixelFormat(PixelComponents::R, PixelDatatype::UNSIGNED_BYTE));
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
		m_sdfAtlasTexture->setFiltering(TextureFiltering::LINEAR);

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

	RenderTarget2D* m_cachedRenderedText;

	static VertexFormat s_vertexFormat;
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
