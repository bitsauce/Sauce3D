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

/** Global FreeType library object */
FT_Library g_library;

/**
 * Glyph descriptor
 */
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

	friend ByteStreamOut& operator<<(ByteStreamOut& out, const GlyphDesc& glyphDesc)
	{
		out << glyphDesc.charcode;
		out << glyphDesc.glyphIndex;
		out << glyphDesc.pixelSize;
		out << glyphDesc.pixelPos;
		out << glyphDesc.pixelDrawOffset;
		out << glyphDesc.advance;
		out << glyphDesc.uv0;
		out << glyphDesc.uv1;
		return out;
	}

	friend ByteStreamIn& operator>>(ByteStreamIn& in, GlyphDesc& glyphDesc)
	{
		in >> glyphDesc.charcode;
		in >> glyphDesc.glyphIndex;
		in >> glyphDesc.pixelSize;
		in >> glyphDesc.pixelPos;
		in >> glyphDesc.pixelDrawOffset;
		in >> glyphDesc.advance;
		in >> glyphDesc.uv0;
		in >> glyphDesc.uv1;
		return in;
	}
};

/**
 * This class contains font data that is slow to generate
 * (e.g. glyph atlases and glyph descriptors.)
 *
 * This data is shared between FontRenderers to imporve performance.
 */
class FontRendererSharedData : public SauceObject
{
public:
	// Reuse FontRenderer's descriptor object
	using FontRendererSharedDataDesc = FontRendererDesc;
	SAUCE_REF_TYPE(FontRendererSharedData);

	FontRendererSharedData()
		: m_fontSize(0)
		, m_fontPadding(40)
		, m_sdfRadius(20)
		, m_subdivisionsPerSDFPixel(4)
		, m_sdfTextureAtlas(nullptr)
	{
	}

	/**
	 * Initializes a font by loading and storing glyph metrics and
	 * renders the font's glyphs to a texture atlas
	 */
	virtual bool initialize(FontRendererSharedDataDesc fontDesc)
	{
		// Load font
		FT_Face face;
		FT_Error error = FT_New_Face(g_library, fontDesc.fontFilePath.c_str(), 0, &face);
		if (error)
		{
			LOG("Failed create font typeface (error string: \"%s\")", FT_Error_String(error));
			return false;
		}

		// Set font size
		error = FT_Set_Char_Size(face, 0, fontDesc.fontSize * 64, 0, 96);
		if (error)
		{
			LOG("Failed create set font size (error string: \"%s\")", FT_Error_String(error));
			return false;
		}
		m_fontSize = fontDesc.fontSize;
		updateGlyphAtlas(face);

		FT_Done_Face(face);

		return true;
	}

	/**
	 * Serialization
	 */
	friend ByteStreamOut& operator<<(ByteStreamOut& out, const FontRendererSharedDataRef& sharedData)
	{
		out << sharedData->m_fontSize;
		out << sharedData->m_fontPadding;
		out << sharedData->m_sdfRadius;
		out << sharedData->m_sdfTextureAtlas;
		out << sharedData->m_subdivisionsPerSDFPixel;
		out << sharedData->m_charcodeToGlyph;
		return out;
	}

	/**
	 * Deserialization
	 */
	friend ByteStreamIn& operator>>(ByteStreamIn& in, FontRendererSharedDataRef& sharedData)
	{
		assert(sharedData == nullptr);
		sharedData = FontRendererSharedDataRef(new FontRendererSharedData());

		in >> sharedData->m_fontSize;
		in >> sharedData->m_fontPadding;
		in >> sharedData->m_sdfRadius;
		in >> sharedData->m_sdfTextureAtlas;
		in >> sharedData->m_subdivisionsPerSDFPixel;
		in >> sharedData->m_charcodeToGlyph;
		return in;
	}

	/**
	 * Getters
	 */
	inline const GlyphDesc* getGlyphDesc(uint64 charcode) const
	{
		const unordered_map<uint64, GlyphDesc>::const_iterator itr = m_charcodeToGlyph.find(charcode);
		if (itr != m_charcodeToGlyph.end())
		{
			return &itr->second;
		}
		return nullptr;
	}

	inline Texture2DRef getSDFTextureAtlas() const
	{
		return m_sdfTextureAtlas;
	}

private:
	/**
	 * Render glyphs to atlas and store glyph metrics
	 */
	void updateGlyphAtlas(FT_Face face)
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
				const uint32 glyphIndex = FT_Get_Char_Index(face, charcode);
				if (glyphIndex <= 0)
				{
					// Skip invalid glyphs
					continue;
				}

				FT_Error error = FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
				if (error)
				{
					LOG("Failed to load glyph for charcode=%l (error string: \"%s\")", charcode, FT_Error_String(error));
					continue;
				}

				const FT_Glyph_Metrics metrics = face->glyph->metrics;
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
					glyphDesc.pixelPos = currentOffset;
					glyphDesc.pixelSize.set(glyphWidth, glyphHeight);
					glyphDesc.advance.set(face->glyph->advance.x / 64, face->glyph->advance.y / 64);
					glyphDesc.pixelDrawOffset.set(metrics.horiBearingX / 64, -metrics.horiBearingY / 64);
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
		for (unordered_map<uint64, GlyphDesc>::iterator itr = m_charcodeToGlyph.begin(); itr != m_charcodeToGlyph.end(); ++itr)
		{
			GlyphDesc& glyphDesc = itr->second;
			FT_Load_Glyph(face, glyphDesc.glyphIndex, FT_LOAD_DEFAULT);
			FT_Error error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_MONO);
			if (error)
			{
				LOG("Failed to render glyph for glyphIndex=%i (error string: \"%s\")", glyphDesc.glyphIndex, FT_Error_String(error));
				continue;
			}
			drawBitmapToAtlas(glyphAtlasData, extents.x, &face->glyph->bitmap, glyphDesc.pixelPos.x + m_fontPadding / 2, glyphDesc.pixelPos.y + m_fontPadding / 2);
			glyphDesc.uv0 = Vector2F(glyphDesc.pixelPos) / Vector2F(extents);
			glyphDesc.uv1 = Vector2F(glyphDesc.pixelPos + glyphDesc.pixelSize) / Vector2F(extents);
		}

		// Create sdf map
		const int32 sdfMapSizeX = extents.x / m_subdivisionsPerSDFPixel;
		const int32 sdfMapSizeY = extents.y / m_subdivisionsPerSDFPixel;
		Pixmap sdfAtlas(sdfMapSizeX, sdfMapSizeY, PixelFormat(PixelComponents::R, PixelDatatype::Uint8));
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
							sdfAtlas.setPixel(sdfMapX, sdfMapY, &sdfPixelValue);
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
						const float distanceNorm = ((signedDistanceNorm + 1.0f) / 2.0f);                   // [+0, +1]
						const uint8 sdfPixelValue = distanceNorm * 255.5f;                                  // [+0, +255]
						sdfAtlas.setPixel(sdfMapX, sdfMapY, &sdfPixelValue);
					}
				}
			}
		}

		Texture2DDesc textureDesc;
		textureDesc.pixmap = &sdfAtlas;
		textureDesc.filtering = TextureFiltering::Linear;
		m_sdfTextureAtlas = CreateNew<Texture2D>(textureDesc);

		delete[] glyphAtlasData;
	}

	/**
	 * Copies a monochrome glyph into a texutre atlas
	 */
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

private:
	/**
	 * Member variables
	 */
	int32 m_fontSize;
	int32 m_fontPadding;
	int32 m_sdfRadius;
	int32 m_subdivisionsPerSDFPixel;
	Texture2DRef m_sdfTextureAtlas;
	unordered_map<uint64, GlyphDesc> m_charcodeToGlyph;
};
SAUCE_REF_TYPE_TYPEDEFS(FontRendererSharedData);

/**
 * Globals used by FontRendererImpl
 */
VertexFormat                                     g_fontVertexFormat;
unordered_map<string, string>                    g_sharedFontDataOnDisk;
unordered_map<string, FontRendererSharedDataRef> g_sharedFontDataLoaded;

const string g_fontShaderVS =
	"in vec2 in_Position;\n"
	"in vec2 in_TexCoord;\n"
	"in vec4 in_VertexColor;\n"
	"\n"
	"out vec2 v_TexCoord;\n"
	"out vec4 v_VertexColor;\n"
	"\n"
	"uniform mat4 u_ModelViewProj;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	gl_Position = vec4(in_Position, 0.0, 1.0) * u_ModelViewProj;\n"
	"	v_TexCoord = in_TexCoord;\n"
	"	v_VertexColor = in_VertexColor;\n"
	"}\n";

const string g_fontShaderPS =
	"in vec2 v_TexCoord;\n"
	"in vec4 v_VertexColor;\n"
	"\n"
	"out vec4 out_FragColor;\n"
	"\n"
	"uniform sampler2D u_Texture;\n"
	"uniform float u_Edge0;\n"
	"uniform float u_Edge1;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	// TODO: Use dFdx(v_TexCoord.x), dFdy(v_TexCoord.y) to shift the edge when font is far away/small\n"
	"	float sdfValue = texture(u_Texture, v_TexCoord).r;\n"
	"	float alpha = smoothstep(u_Edge0, u_Edge1, sdfValue);\n"
	"	out_FragColor = vec4(v_VertexColor.rgb, alpha);\n"
	"}\n";

ShaderRef g_fontShader;

//--------------------------------------------------------------
// FontRenderingSystem::Initialize()
//--------------------------------------------------------------
bool FontRenderingSystem::Initialize(GraphicsContextRef context)
{
	FT_Error error = FT_Init_FreeType(&g_library);
	if (error)
	{
		LOG("Failed to initialize the FontRenderingSystem (error string: \"%s\")", FT_Error_String(error));
		return false;
	}

	// Create shader from strings
	ShaderDesc shaderDesc;
	shaderDesc.shaderSourceVS = g_fontShaderVS;
	shaderDesc.shaderSourcePS = g_fontShaderPS;
	g_fontShader = CreateNew<Shader>(shaderDesc);

	// Set font vertex format
	g_fontVertexFormat.set(VertexAttribute::Position, 2, Datatype::Float);
	g_fontVertexFormat.set(VertexAttribute::TexCoord, 2, Datatype::Float);
	g_fontVertexFormat.set(VertexAttribute::Color, 4, Datatype::Uint8);

	// Register all cached fonts
	util::FileSystemIterator cachedFontsDir("DataCache/Fonts", "*", (uint32)util::FileSystemIteratorFlag::IncludeFiles);
	for (util::DirectoryOrFile cachedFontFile : cachedFontsDir)
	{
		g_sharedFontDataOnDisk[cachedFontFile.baseName] = cachedFontFile.fullPath;
	}

	return true;
}

void FontRenderingSystem::Free()
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
		: m_sharedData(nullptr)
		, m_indices(nullptr)
	{
	}

	~FontRendererImpl()
	{
		delete[] m_indices;
	}

	bool initialize(FontRendererDesc fontDesc) override
	{
		if (fontDesc.fontFilePath.empty())
		{
#ifdef SAUCE_COMPILE_WINDOWS
			fontDesc.fontFilePath = "C:/Windows/Fonts/Arial.ttf";
#else
			LOG("No default font provided for the current platform");
			return false;
#endif
		}

		// Check if shared font data has already been loaded
		const string cachedFontKey = fontDesc.getKey();
		{
			const unordered_map<string, FontRendererSharedDataRef>::iterator itr = g_sharedFontDataLoaded.find(cachedFontKey);
			if (itr != g_sharedFontDataLoaded.end())
			{
				m_sharedData = itr->second;
				return true;
			}
		}

		// Check if we have a cached version of this font's shared data on disk
		{
			const unordered_map<string, string>::iterator itr = g_sharedFontDataOnDisk.find(cachedFontKey);
			if (itr != g_sharedFontDataOnDisk.end())
			{
				if (loadCached(itr->second))
				{
					g_sharedFontDataLoaded[cachedFontKey] = m_sharedData;
					return true;
				}
				else
				{
					LOG("Failed to load font's cached shared data from file");
					return false;
				}
			}
		}

		// Couldn't find cached shared font data, initialize new font
		{
			m_sharedData = sauce::CreateNew<FontRendererSharedData>(fontDesc);

			// Write to data cache
			if (!filesystem::exists("DataCache/Fonts/"))
			{
				filesystem::create_directories("DataCache/Fonts/");
			}
			saveCached("DataCache/Fonts/" + fontDesc.getKey());

			g_sharedFontDataLoaded[cachedFontKey] = m_sharedData;

			return true;
		}
	}

	bool loadCached(const string& cachedFile)
	{
		ByteStreamIn fileStream(cachedFile);
		if (fileStream)
		{
			fileStream >> m_sharedData;
			fileStream.close();
			return true;
		}
		return false;
	}

	bool saveCached(const string& cachedFile)
	{
		ByteStreamOut fileStream(cachedFile);
		if (fileStream)
		{
			fileStream << m_sharedData;
			fileStream.close();
			return true;
		}
		return false;
	}

	void drawText(GraphicsContextRef context, FontRendererDrawTextArgs& args) override
	{
		const int32 numChars = args.text.length();

		// We only regenerate vertex buffers if text has changed
		const bool hasTextChanged = m_prevDrawArgs.text != args.text;
		if (hasTextChanged)
		{
			m_extentsOfString = Vector2F::Zero;

			delete[] m_indices;
			m_vertices = g_fontVertexFormat.createVertices(numChars * 4);
			m_indices = new uint32[numChars * 6];

			Vector2F currentPos = Vector2F(0.0f, 0.0f);

			for (int32 i = 0; i < numChars; ++i)
			{
				// Find glyph desc
				const GlyphDesc* glyphDesc;
				{
					const uint64 charcode = args.text[i];
					glyphDesc = m_sharedData->getGlyphDesc(charcode);
					if (!glyphDesc)
					{
						LOG("Tried to render glyph with charcode '%i', but no maching glyph descriptor was found", charcode);
						continue;
					}
				}

				const Vector2F currentTL = currentPos + glyphDesc->pixelDrawOffset;
				const Vector2F currentBR = currentPos + glyphDesc->pixelSize + glyphDesc->pixelDrawOffset;
				m_vertices[i * 4 + 0].set2f(VertexAttribute::Position, currentTL.x, currentTL.y);
				m_vertices[i * 4 + 0].set2f(VertexAttribute::TexCoord, glyphDesc->uv0.x, glyphDesc->uv0.y);
				m_vertices[i * 4 + 0].set4ub(VertexAttribute::Color, 255, 255, 255, 255);

				m_vertices[i * 4 + 1].set2f(VertexAttribute::Position, currentBR.x, currentTL.y);
				m_vertices[i * 4 + 1].set2f(VertexAttribute::TexCoord, glyphDesc->uv1.x, glyphDesc->uv0.y);
				m_vertices[i * 4 + 1].set4ub(VertexAttribute::Color, 255, 255, 255, 255);

				m_vertices[i * 4 + 2].set2f(VertexAttribute::Position, currentTL.x, currentBR.y);
				m_vertices[i * 4 + 2].set2f(VertexAttribute::TexCoord, glyphDesc->uv0.x, glyphDesc->uv1.y);
				m_vertices[i * 4 + 2].set4ub(VertexAttribute::Color, 255, 255, 255, 255);

				m_vertices[i * 4 + 3].set2f(VertexAttribute::Position, currentBR.x, currentBR.y);
				m_vertices[i * 4 + 3].set2f(VertexAttribute::TexCoord, glyphDesc->uv1.x, glyphDesc->uv1.y);
				m_vertices[i * 4 + 3].set4ub(VertexAttribute::Color, 255, 255, 255, 255);

				m_indices[i * 6 + 0] = i * 4 + 0;
				m_indices[i * 6 + 1] = i * 4 + 2;
				m_indices[i * 6 + 2] = i * 4 + 1;

				m_indices[i * 6 + 3] = i * 4 + 3;
				m_indices[i * 6 + 4] = i * 4 + 1;
				m_indices[i * 6 + 5] = i * 4 + 2;

				currentPos += glyphDesc->advance;

			}
			m_extentsOfString = currentPos;
		}

		// Setup shader
		{
			context->setShader(g_fontShader);
			g_fontShader->setUniform1f("u_Edge0", args.edge0);
			g_fontShader->setUniform1f("u_Edge1", args.edge1);
			g_fontShader->setSampler2D("u_Texture", m_sharedData->getSDFTextureAtlas());
		}

		Matrix4 drawTransform;
		if (args.transform == Matrix4::Zero)
		{
			Vector2F centering = Vector2F::Zero;
			if (args.alignment == TextAlignment::Centered)
			{
				centering.x = -0.5f;
			}
			else if (args.alignment == TextAlignment::Right)
			{
				centering.x = -1.0f;
			}

			drawTransform.scale(1.0f / m_extentsOfString.x);
			drawTransform.translate(centering.x, centering.y, 0.0f);
			drawTransform.scale(m_extentsOfString.x * args.scale);
			drawTransform.rotateZ(args.rotation);
			drawTransform.translate(args.position.x, args.position.y, 0.0f);
		}
		else
		{
			drawTransform = args.transform;
		}

		// Draw text
		context->pushMatrix(drawTransform);
		context->drawIndexedPrimitives(PrimitiveType::Triangles, m_vertices, numChars * 4, m_indices, numChars * 6);
		context->popMatrix();

		// Clean up
		context->setTexture(nullptr);
		context->setShader(nullptr);

		m_prevDrawArgs = args;
	}

private:
	FontRendererSharedDataRef m_sharedData;
	FontRendererDrawTextArgs m_prevDrawArgs;
	
	Vector2F m_extentsOfString;

	VertexArray m_vertices;
	uint32* m_indices;

	static VertexFormat s_vertexFormat;
};

FontRenderer* FontRenderer::CreateImpl()
{
	return new FontRendererImpl();
}

END_SAUCE_NAMESPACE

